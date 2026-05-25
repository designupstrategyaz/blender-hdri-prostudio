#include "plugin.h"
#include "image.h"
#include "python.h"
#include "border.h"
#include "multithreading.h"

#include <vector>

using namespace std;

void init(
      void* (*image)()
    , void* (*curveMappingFlat)(int)
    , void* (*curveMappingImage)(int)
    , void* (*colorRamp)(int)
    , void* (*lightImage)(int)
)
{
    Python::image = image;

    Python::curveMappingFlat = curveMappingFlat;
    Python::curveMappingImage = curveMappingImage;

    Python::colorRamp = colorRamp;
    Python::lightImage = lightImage;


    Border::init();
    Multithreading::init();
    Light::init();
}

void initImage()
{
    new Image_;
}

void createFinalImage()
{
    g_image->update();
}

void addLight(
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
{
    Light::list().push_back(new Light(
          enabled
        , solo
        , blendMode
        , brightnessColor
        , brightnessOpacity
        , curveX
        , curveY
        , x
        , y
        , width
        , height
        , angle
        , isEllipse
        , sides
        , cornerRadius
        , contentType
        , color
        , falloffType
        , gradientAngle
        , radialX
        , radialY
        , radialWidth
        , radialHeight
    ));
}

void changeLightEnabled(int index, bool enabled)
{
    Light::list()[index]->setEnabled(enabled);
}

void changeLightSolo(int index, bool solo)
{
    Light::list()[index]->setSolo(solo);
}

void changeLightBlendMode(int index, BlendMode blendMode)
{
    Light::list()[index]->setBlendMode(blendMode);
}

void changeLightBrightnessColor(int index, float brightnessColor)
{
    Light::list()[index]->setBrightnessColor(brightnessColor);
}

void changeLightBrightnessOpacity(int index, float brightnessOpacity)
{
    Light::list()[index]->setBrightnessOpacity(brightnessOpacity);
}

void changeLightCurveX(int index, float curveX)
{
    Light::list()[index]->setCurveX(curveX);
}

void changeLightCurveY(int index, float curveY)
{
    Light::list()[index]->setCurveY(curveY);
}

void changeBothCoordinates(int index)
{
    Light::list()[index]->changeBothCoordinates();
}

void changeLightX(int index, int x)
{
    Light::list()[index]->setX(x);
}

void changeLightY(int index, int y)
{
    Light::list()[index]->setY(y);
}

void changeLightWidth(int index, int width)
{
    Light::list()[index]->setWidth(width);
}

void changeLightHeight(int index, int height)
{
    Light::list()[index]->setHeight(height);
}

void changeLightAngle(int index, int angle)
{
    Light::list()[index]->setAngle(angle);
}

void changeLightIsEllipse(int index, bool isEllipse)
{
    Light::list()[index]->setIsEllipse(isEllipse);
}

void changeLightSides(int index, int sides)
{
    Light::list()[index]->setSides(sides);
}

void changeLightCornerRadius(int index, float cornerRadius)
{
    Light::list()[index]->setCornerRadius(cornerRadius);
}

void changeLightContentType(int index, ContentType contentType)
{
    Light::list()[index]->setContentType(contentType);
}

void changeLightColor(int index, float *color)
{
    Light *light = Light::list()[index];

    light->setColor(color);
    light->drawColor();
}

void changeLightFalloffType(int index, FalloffType falloffType)
{
    Light::list()[index]->setFalloffType(falloffType);
}

void changeLightGradientAngle(int index, int gradientAngle)
{
    Light::list()[index]->setGradientAngle(gradientAngle);
}

void changeLightRadialX(int index, float radialX)
{
    Light::list()[index]->setRadialX(radialX);
}

void changeLightRadialY(int index, float radialY)
{
    Light::list()[index]->setRadialY(radialY);
}

void changeLightRadialWidth(int index, float radialWidth)
{
    Light::list()[index]->setRadialWidth(radialWidth);
}

void changeLightRadialHeight(int index, float radialHeight)
{
    Light::list()[index]->setRadialHeight(radialHeight);
}

void updateLightImage(int index)
{
    Light *light = Light::list()[index];

    light->updateImage();
    light->drawColor();
}

void changeLightImageBrightness(int index)
{
    Light *light = Light::list()[index];

    light->updateImageBrightness();
    light->drawColor();
}

void removeLight(int index)
{
    vector<Light*> &list = Light::list();

    delete list[index];
    list.erase(list.begin() + index);

    for(int i = index; i < list.size(); i++)
        list[i]->decrementIndex();
}

void up(int index)
{
    vector<Light*> &list = Light::list();
    const int upper = index - 1;

    Light *tmp = list[index];
    list[index] = list[upper];
    list[upper] = tmp;

    list[index]->incrementIndex();
    list[upper]->decrementIndex();
}

void down(int index)
{
    vector<Light*> &list = Light::list();
    const int lower = index + 1;

    Light *tmp = list[index];
    list[index] = list[lower];
    list[lower] = tmp;

    list[index]->decrementIndex();
    list[lower]->incrementIndex();
}

bool lightIsVisible(int index)
{
    return g_image->lightIsVisible(index);
}

bool curveFlatChanged(int index)
{
    return Light::list()[index]->curveFlatChanged();
}

bool curveImageChanged(int index)
{
    return Light::list()[index]->curveImageChanged();
}

bool colorRampChanged(int index)
{
    return Light::list()[index]->colorRampChanged(index);
}


void updateLightLuminosity(int index)
{
    Light::list()[index]->drawLuminosity();
}

void updateLightColor(int index)
{
    Light::list()[index]->drawColor();
}

void cleanImage()
{
    g_image->clean();
}

void clearImage()
{
    delete g_image;
}

void clear()
{
    clearImage();

    vector<Light*> &list = Light::list();

    for(int i = 0; i < list.size(); i++)
        delete list[i];

    list.clear();
}
