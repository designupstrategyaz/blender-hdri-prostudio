#include "light.h"
#include "image.h"
#include "blender.h"
#include "math_utils.h"
#include "python.h"
#include "mipmap.h"
#include "scale.h"
#include "geometry.h"
#include "texturedrawer.h"
#include "distanceonsphere.h"
#include "lightplane.h"
#include "border.h"
#include "multithreading.h"

using namespace std;

vector<Light*> Light::_list;

std::vector<float*> Light::_borderLuminosity;
std::vector<float*> Light::_borderColor;

bool Light::_renderLuminosity;
bool Light::_renderColor;

void Light::init()
{
    const int threads = Multithreading::threads();

    _borderLuminosity.resize(threads);
    _borderColor.resize(threads);
}

vector<Light*> &Light::list()
{
    return _list;
}

const vector<Light*> &Light::const_list()
{
    return _list;
}

Light::Light(
      bool enabled
    , bool solo
    , BlendMode blendMode
    , float brightnessColor
    , float brightnessOpacity
    , float curveX
    , float curveY
    , int x
    , int y
    , int width
    , int height
    , int angle
    , bool isEllipse
    , int sides
    , float cornerRadius
    , ContentType contentType
    , float *color
    , FalloffType falloffType
    , int gradientAngle
    , float radialX
    , float radialY
    , float radialWidth
    , float radialHeight
)
    : _index(_list.size())
    , _position(x, y)
{
    _enabled = enabled;
    _solo = solo;

    _blendMode = blendMode;

    _brightnessColor = brightnessColor;
    _brightnessOpacity = brightnessOpacity;

    _curveX = curveX;
    _curveY = curveY;

    _curveFlatTime = Blender::curveChangedTime(Python::curveMappingFlat(_index));
    _curveImageTime = Blender::curveChangedTime(Python::curveMappingImage(_index));


    _geometry = new Geometry(this);
    _border = new Border(this);

    _width = toRadians(width);
    _height = toRadians(height);

    _angle = toRadians(angle);


    _isEllipse = isEllipse;

    _sides = sides;
    _cornerRadius = cornerRadius;




    _contentType = contentType;



    setColor(color);



    _colorRampPrevious = new Blender::ColorRampMinimal;
    copyColorRamp(reinterpret_cast<const Blender::ColorBand*>(Python::colorRamp(_index)));


    _falloffType = falloffType;

    _gradientAngle = toRadians(gradientAngle);

    _radialX = radialX;
    _radialY = radialY;
    _radialWidth = radialWidth;
    _radialHeight = radialHeight;



    _imageEdited = 0;
    _imagePixelsOriginal = 0;
    _imageBrightnessOriginal = 0;

    updateImage();




    const size_t size = g_image->size();

    _luminosityCanvas = new float[size];
    _pixels = new float[g_image->pixelsSize()];

    _changeBothCoordinates = false;



    update();
}

Light::~Light()
{
    delete[] _pixels;
    delete[] _luminosityCanvas;

    deleteimageBrightnessOriginal();
    deleteImagePixelsOriginal();
    deleteImageEdited();

    delete _colorRampPrevious;
    delete _border;
    delete _geometry;
}

void Light::update()
{
    if(!_border->find())
        return;

    for(size_t i = 0; i < g_image->size(); i++)
        _luminosityCanvas[i] = 0;


    _renderLuminosity = true;
    _renderColor = true;

    execRender();
}

void Light::drawLuminosity()
{
    _renderLuminosity = true;
    _renderColor = false;

    execRender();
}

void Light::drawColor()
{
    _renderLuminosity = false;
    _renderColor = true;

    execRender();
}

void Light::scale(int inX, int inY, int outX, int outY)
{
    Scale::downscale2x(_mipmaps[inY][inX], _mipmaps[outY][outX]);
}

void Light::updateImageBrightness()
{
    _image = Python::lightImage(_index);

    if(!_image)
        return;


    Multithreading::exec(transformBrightness, Python::curveMappingImage(_index), this);

    const int mp2 = g_image->drawer()->mp2();
    Scale::general(_imageEdited, _mipmaps[mp2][mp2]);

    for(int i = mp2 - 1; i >= 0; i--)
        scale(i + 1, mp2, i, mp2);

    for(int i = mp2 - 1; i >= 0; i--) {
        for(int j = 0; j < mp2 + 1; j++)
            scale(j, i + 1, j, i);
    }
}

void Light::incrementIndex()
{
    _index++;
}

void Light::decrementIndex()
{
    _index--;
}

void Light::render(int id, const void *unused, void *light)
{
    for(int i = id; i < g_image->height(); i += Multithreading::threads()) {
        for(int j = 0; j < g_image->width(); j++)
            ((Light*)light)->renderPixel(j, i, id);
    }
}

void Light::transformBrightness(int id, const void *curveMappingImage, void *light)
{
    Light *light_ = (Light*)light;

    Picture *imageEdited = light_->_imageEdited;
    const float *imagePixelsOriginal = light_->_imagePixelsOriginal;

    for(size_t i = id; i < imageEdited->size(); i += Multithreading::threads()) {
        const int index = i * 4;

        const float brightnessOriginal = light_->_imageBrightnessOriginal[i];
        const float scale = brightnessOriginal == 0 ? 1 : Blender::curvemappingEvaluateF(curveMappingImage, brightnessOriginal) / brightnessOriginal;


        for(int k = 0; k < 3; k++)
            imageEdited->pixels()[index + k] = imagePixelsOriginal[index + k] * scale;

        imageEdited->pixels()[index + 3] = imagePixelsOriginal[index + 3];
    }
}

void Light::renderPixel(int x, int y, int id)
{
    const size_t index = g_image->coordinate(x, y);

    switch(_border->stateCanvas()[index]) {
    case Outside:
        return;

    case Inside: {
        float luminosity, color[4];
        computePoint(x + 0.5f, y + 0.5f, &luminosity, color);

        size_t index = g_image->coordinate(x, y);


        if(_renderLuminosity)
            _luminosityCanvas[index] = luminosity;

        if(_renderColor) {
            for(int i = 0; i < 4; i++)
                _pixels[index * 4 + i] = color[i];
        }

        break;
    }
    case Border_:
        if(_renderLuminosity) {
            _borderLuminosity[id] = &_luminosityCanvas[index];
            *(_borderLuminosity[id]) = 0;
        }

        if(_renderColor) {
            _borderColor[id] = &_pixels[index * 4];
            zero(_borderColor[id]);
        }


        int count = 0;
        for(int i = 0; i < 4; i++)
            count += subPixel(x + 0.25f + (i / 2) / 2.f, y + 0.25f + (i % 2) / 2.f, id);


        if(_renderColor && count)
            normalize(_borderColor[id], count);


        break;
    }
}

bool Light::subPixel(float x, float y, int id)
{
    float subLuminosity;
    float subColor[4];

    if(!computePoint(x, y, &subLuminosity, subColor))
        return false;


    if(_renderLuminosity)
        *(_borderLuminosity[id]) += subLuminosity / 4;

    if(_renderColor)
        addWeighted(_borderColor[id], subColor);


    return true;
}

bool Light::computePoint(float x, float y, float *luminosity, float color[4])
{
    Vector3f distance;
    DistanceOnSphere d(this, x, y);
    d.compute(&distance);

    LightPlane l(this, distance);
    const float rScaled = l.rScaled();

    if(!l.insideShape())
        return false;

    if(_renderLuminosity) {
        const double dx = l.xScaled() - _curveX;
        const double dy = l.yScaled() - _curveY;

        const float r = sqrt(dx * dx + dy * dy);
        *luminosity = Blender::curvemappingEvaluateF(_curveMappingFlat, r);
    }

    if(_renderColor) {
        switch(_contentType) {
        case Flat:
            for(int i = 0; i < 3; i++)
                color[i] = _color[i];

            color[3] = 1;
            break;

        case Gradient:
            float gradient;

            switch(_falloffType) {
            case Linear:
                gradient = (rScaled * cos(l.aScaled() - _gradientAngle) + 1) / 2;
                break;

            case Radial: {
                const double dx = (l.xScaled() - _radialX) / _radialWidth;
                const double dy = (l.yScaled() - _radialY) / _radialHeight;

                gradient = sqrt(dx * dx + dy * dy);
                break;
            }
            case Conical:
                double a = l.aScaled() - _gradientAngle;

                while(a < 0)
                    a += 2 * M_PI;
                while(a >= 2 * M_PI)
                    a -= 2 * M_PI;

                gradient = a / 2 / M_PI;
                break;
            }

            computeGradientColor(gradient, color);
            break;

        case Image:
            if(_image)
                g_image->drawer()->point(d.scalePointVector(), l, color);
            else {
                for(int i = 0; i < 4; i++)
                    color[i] = 0;
            }

            break;
        }
    }

    return true;
}

void Light::computeGradientColor(float gradient, float color[4]) const
{
    Blender::colorbandEvaluate(_colorRamp, gradient, color);

    for(int i = 0; i < 3; i++)
        color[i] *= color[3];
}

bool Light::enabled() const
{
    return _enabled;
}

bool Light::solo() const
{
    return _solo;
}

BlendMode Light::blendMode() const
{
    return _blendMode;
}

const float *Light::luminosity() const
{
    return _luminosityCanvas;
}

float Light::brightnessColor() const
{
    return _brightnessColor;
}

float Light::brightnessOpacity() const
{
    return _brightnessOpacity;
}

Vector2i Light::position() const
{
    return _position;
}

float Light::angle() const
{
    return _angle;
}

float Light::width() const
{
    return _width;
}

float Light::height() const
{
    return _height;
}

bool Light::isEllipse() const
{
    return _isEllipse;
}

int Light::sides() const
{
    return _sides;
}

float Light::cornerRadius() const
{
    return _cornerRadius;
}

const float *Light::pixels() const
{
    return _pixels;
}

const std::vector<std::vector<Mipmap*> > &Light::mipmaps() const
{
    return _mipmaps;
}

const Geometry *Light::geometry() const
{
    return _geometry;
}

const Border *Light::border() const
{
    return _border;
}

bool Light::curveFlatChanged()
{
    const int newTime = Blender::curveChangedTime(Python::curveMappingFlat(_index));
    const bool changed = newTime > _curveFlatTime;
    _curveFlatTime = newTime;
    return changed;
}

bool Light::curveImageChanged()
{
    const int newTime = Blender::curveChangedTime(Python::curveMappingImage(_index));
    const bool changed = newTime > _curveImageTime;
    _curveImageTime = newTime;
    return changed;
}

bool Light::colorRampChanged(int index)
{
    const Blender::ColorBand *colorRamp = reinterpret_cast<const Blender::ColorBand*>(Python::colorRamp(_index));

    if(*colorRamp == *_colorRampPrevious)
        return false;

    copyColorRamp(colorRamp);
    return true;
}

void Light::setEnabled(bool enabled)
{
    _enabled = enabled;
}

void Light::setSolo(bool solo)
{
    _solo = solo;
}

void Light::setBlendMode(BlendMode blendMode)
{
    _blendMode = blendMode;
}

void Light::setBrightnessColor(float brightnessColor)
{
    _brightnessColor = brightnessColor;
}

void Light::setBrightnessOpacity(float brightnessOpacity)
{
    _brightnessOpacity = brightnessOpacity;
}

void Light::setCurveX(float curveX)
{
    _curveX = curveX;
    drawLuminosity();
}

void Light::setCurveY(float curveY)
{
    _curveY = curveY;
    drawLuminosity();
}

void Light::changeBothCoordinates()
{
    _changeBothCoordinates = true;
}

void Light::setPosition(Vector2i position)
{
    _position = position;
}

void Light::setX(int x)
{
    if(_changeBothCoordinates)
        _moveX = x;
    else {
        _geometry->move(Vector2i(x, _position.y()));
        update();
    }
}

void Light::setY(int y)
{
    _geometry->move(Vector2i(_changeBothCoordinates ? _moveX : _position.x(), y));

    if(_changeBothCoordinates)
        _changeBothCoordinates = false;

    update();
}

void Light::setWidth(int width)
{
    _width = toRadians(width);
    update();
}

void Light::setHeight(int height)
{
    _height = toRadians(height);
    update();
}

void Light::setAngle(int angle)
{
    const float newAngle = toRadians(angle);
    _geometry->rotate(newAngle);
    _angle = newAngle;
    update();
}

void Light::setIsEllipse(bool isEllipse)
{
    _isEllipse = isEllipse;
    update();
}

void Light::setSides(int sides)
{
    _sides = sides;
    update();
}

void Light::setCornerRadius(float cornerRadius)
{
    _cornerRadius = cornerRadius;
    update();
}

void Light::setContentType(ContentType contentType)
{
    _contentType = contentType;
    drawColor();
}

void Light::setColor(float *color)
{
    for(int i = 0; i < 3; i++)
        _color[i] = color[i];
}

void Light::setFalloffType(FalloffType falloffType)
{
    _falloffType = falloffType;
    drawColor();
}

void Light::setGradientAngle(int gradientAngle)
{
    _gradientAngle = toRadians(gradientAngle);
    drawColor();
}

void Light::setRadialX(float radialX)
{
    _radialX = radialX;
    drawColor();
}

void Light::setRadialY(float radialY)
{
    _radialY = radialY;
    drawColor();
}

void Light::setRadialWidth(float radialWidth)
{
    _radialWidth = radialWidth;
    drawColor();
}

void Light::setRadialHeight(float radialHeight)
{
    _radialHeight = radialHeight;
    drawColor();
}

void Light::updateImage()
{
    _image = Python::lightImage(_index);

    if(!_image)
        return;


    deleteImageEdited();
    _imageEdited = new Picture(Blender::width(_image), Blender::height(_image));

    deleteImagePixelsOriginal();
    _imagePixelsOriginal = new float[_imageEdited->pixelsSize()];

    deleteimageBrightnessOriginal();
    _imageBrightnessOriginal = new float[_imageEdited->size()];


    for(int i = 0; i < _imageEdited->height(); i++) {
        for(int j = 0; j < _imageEdited->width(); j++) {
            float colorVectorSquare = 0;
            const size_t index = _imageEdited->coordinate(j, i);

            for(int k = 0; k < 3; k++) {
                float &channel = _imagePixelsOriginal[index * 4 + k];

                channel = Blender::getColorChannel(_image, j, i, k);
                colorVectorSquare += channel * channel;
            }

            _imagePixelsOriginal[index * 4 + 3] = Blender::getColorChannel(_image, j, i, 3);

            _imageBrightnessOriginal[index] = sqrt(colorVectorSquare / 3);
        }
    }

    for(auto &line : _mipmaps) {
        for(auto map : line)
            delete map;
    }

    const TextureDrawer *drawer = g_image->drawer();
    const int mp2 = drawer->mp2();
    const int maxValue = drawer->maxValue();
    const int count = mp2 + 1;

    _mipmaps.resize(count);

    for(int i = 0; i < count; i++) {
        auto &line = _mipmaps[i];
        line.resize(count);

        const int sizeY = pow2(i);
        const float scaleY = float(sizeY) / maxValue;

        for(int j = 0; j < count; j++) {
            const int sizeX = pow2(j);
            const float scaleX = float(sizeX) / maxValue;

            line[j] = new Mipmap(sizeX, sizeY, scaleX, scaleY);
        }
    }

    updateImageBrightness();
}

double Light::toRadians(int a)
{
    return a * M_PI / 180;
}

void Light::copyColorRamp(const Blender::ColorBand *colorRamp)
{
    *_colorRampPrevious = *colorRamp;
}

void Light::deleteImageEdited()
{
    delete _imageEdited;
}

void Light::deleteImagePixelsOriginal()
{
    delete[] _imagePixelsOriginal;
}

void Light::deleteimageBrightnessOriginal()
{
    delete[] _imageBrightnessOriginal;
}

void Light::execRender()
{
    _curveMappingFlat = Python::curveMappingFlat(_index);
    _colorRamp = Python::colorRamp(_index);
    g_image->drawer()->setLight(this);

    Multithreading::exec(render, 0, this);
}
