#include "plugin.h"
#include "image.h"
#include "python.h"
#include "border.h"
#include "multithreading.h"

#include <vector>

using namespace std;


// Guards every index-taking entry point below: a stale light index coming
// from the Python side must not become an out-of-bounds vector access.
static bool validIndex(int index)
{
    return index >= 0 && index < int(Light::const_list().size());
}


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

void deinit()
{
    Multithreading::shutdown();
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
    if(!validIndex(index))
        return;

    Light::list()[index]->setEnabled(enabled);
}

void changeLightSolo(int index, bool solo)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setSolo(solo);
}

void changeLightBlendMode(int index, BlendMode blendMode)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setBlendMode(blendMode);
}

void changeLightBrightnessColor(int index, float brightnessColor)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setBrightnessColor(brightnessColor);
}

void changeLightBrightnessOpacity(int index, float brightnessOpacity)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setBrightnessOpacity(brightnessOpacity);
}

void changeLightCurveX(int index, float curveX)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setCurveX(curveX);
}

void changeLightCurveY(int index, float curveY)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setCurveY(curveY);
}

void changeBothCoordinates(int index)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->changeBothCoordinates();
}

void changeLightX(int index, int x)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setX(x);
}

void changeLightY(int index, int y)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setY(y);
}

void changeLightWidth(int index, int width)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setWidth(width);
}

void changeLightHeight(int index, int height)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setHeight(height);
}

void changeLightAngle(int index, int angle)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setAngle(angle);
}

void changeLightIsEllipse(int index, bool isEllipse)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setIsEllipse(isEllipse);
}

void changeLightSides(int index, int sides)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setSides(sides);
}

void changeLightCornerRadius(int index, float cornerRadius)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setCornerRadius(cornerRadius);
}

void changeLightContentType(int index, ContentType contentType)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setContentType(contentType);
}

void changeLightColor(int index, float *color)
{
    if(!validIndex(index))
        return;

    Light *light = Light::list()[index];

    light->setColor(color);
    light->drawColor();
}

void changeLightFalloffType(int index, FalloffType falloffType)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setFalloffType(falloffType);
}

void changeLightGradientAngle(int index, int gradientAngle)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setGradientAngle(gradientAngle);
}

void changeLightRadialX(int index, float radialX)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setRadialX(radialX);
}

void changeLightRadialY(int index, float radialY)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setRadialY(radialY);
}

void changeLightRadialWidth(int index, float radialWidth)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setRadialWidth(radialWidth);
}

void changeLightRadialHeight(int index, float radialHeight)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->setRadialHeight(radialHeight);
}

void updateLightImage(int index)
{
    if(!validIndex(index))
        return;

    Light *light = Light::list()[index];

    light->updateImage();
    light->drawColor();
}

void changeLightImageBrightness(int index)
{
    if(!validIndex(index))
        return;

    Light *light = Light::list()[index];

    light->updateImageBrightness();
    light->drawColor();
}

void removeLight(int index)
{
    if(!validIndex(index))
        return;

    vector<Light*> &list = Light::list();

    delete list[index];
    list.erase(list.begin() + index);

    for(int i = index; i < int(list.size()); i++)
        list[i]->decrementIndex();
}

void up(int index)
{
    if(!validIndex(index) || index < 1)
        return;

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

    if(!validIndex(index) || index + 1 >= int(list.size()))
        return;

    const int lower = index + 1;

    Light *tmp = list[index];
    list[index] = list[lower];
    list[lower] = tmp;

    list[index]->decrementIndex();
    list[lower]->incrementIndex();
}

bool lightIsVisible(int index)
{
    if(!validIndex(index))
        return false;

    return g_image->lightIsVisible(index);
}

bool curveFlatChanged(int index)
{
    if(!validIndex(index))
        return false;

    return Light::list()[index]->curveFlatChanged();
}

bool curveImageChanged(int index)
{
    if(!validIndex(index))
        return false;

    return Light::list()[index]->curveImageChanged();
}

bool colorRampChanged(int index)
{
    if(!validIndex(index))
        return false;

    return Light::list()[index]->colorRampChanged(index);
}


void updateLightLuminosity(int index)
{
    if(!validIndex(index))
        return;

    Light::list()[index]->drawLuminosity();
}

void updateLightColor(int index)
{
    if(!validIndex(index))
        return;

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

    for(int i = 0; i < int(list.size()); i++)
        delete list[i];

    list.clear();
}
