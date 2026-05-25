#ifndef PLUGIN_H
#define PLUGIN_H

#include "light.h"

#if defined _WIN32 || defined _WIN64
    #define EXPORT extern "C" __declspec(dllexport)
#elif __unix__ || __APPLE__
    #define EXPORT extern "C"
#endif

EXPORT void init(
      void* (*image)()
    , void* (*curveMappingFlat)(int)
    , void* (*curveMappingImage)(int)
    , void* (*colorRamp)(int)
    , void* (*lightImage)(int)
);

EXPORT void initImage();
EXPORT void createFinalImage();


EXPORT void addLight(
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
    , float radialHeigh
);
EXPORT void changeBothCoordinates(int index);


EXPORT void changeLightEnabled(int index, bool enabled);
EXPORT void changeLightSolo(int index, bool solo);

EXPORT void changeLightBlendMode(int index, BlendMode blendMode);

EXPORT void changeLightBrightnessColor(int index, float brightnessColor);
EXPORT void changeLightBrightnessOpacity(int index, float brightnessOpacity);

EXPORT void changeLightCurveX(int index, float curveX);
EXPORT void changeLightCurveY(int index, float curveY);

EXPORT void changeLightX(int index, int x);
EXPORT void changeLightY(int index, int y);

EXPORT void changeLightWidth(int index, int width);
EXPORT void changeLightHeight(int index, int height);

EXPORT void changeLightAngle(int index, int angle);


EXPORT void changeLightIsEllipse(int index, bool isEllipse);

EXPORT void changeLightSides(int index, int sides);
EXPORT void changeLightCornerRadius(int index, float cornerRadius);



EXPORT void changeLightContentType(int index, ContentType contentType);


EXPORT void changeLightColor(int index, float *color);


EXPORT void changeLightFalloffType(int index, FalloffType falloffType);

EXPORT void changeLightGradientAngle(int index, int gradientAngle);

EXPORT void changeLightRadialX(int index, float radialX);
EXPORT void changeLightRadialY(int index, float radialY);
EXPORT void changeLightRadialWidth(int index, float radialWidth);
EXPORT void changeLightRadialHeight(int index, float radialHeight);


EXPORT void updateLightImage(int index);
EXPORT void changeLightImageBrightness(int index);



EXPORT void removeLight(int index);

EXPORT void up(int index);
EXPORT void down(int index);



EXPORT bool lightIsVisible(int index);

EXPORT bool curveFlatChanged(int index);
EXPORT bool curveImageChanged(int index);

EXPORT bool colorRampChanged(int index);


EXPORT void updateLightLuminosity(int index);
EXPORT void updateLightColor(int index);

EXPORT void cleanImage();

EXPORT void clearImage();

EXPORT void clear();


#endif // PLUGIN_H
