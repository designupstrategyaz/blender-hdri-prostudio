#ifndef TEXTUREDRAWER_H
#define TEXTUREDRAWER_H

#include "vector.h"

class LightPlane;
class Light;
class Mipmap;

class TextureDrawer
{
public:
    TextureDrawer();
    void setLight(const Light *light);

    int mp2() const;
    int maxValue() const;

    void point(const float distanceScale, const LightPlane &l, float *color);

private:
    const Light *_light;

    int _mp2;
    int _maxValue;


    void bilinearFiltering(float *outPixel, Vector2f inR, const Mipmap *map);
    void trilinearFiltering(float *outPixel, Vector2f inR, const Mipmap *smallerMap, const Mipmap *biggerMap, float const MipmapDistance);
    void anisotropicFiltering(float *outPixel, Vector2f inR, const Mipmap *yx, const Mipmap *yX, const Mipmap *Yx, const Mipmap *YX, float mipmapDistanceX, float mipmapDistanceY);

    float sc(int r);
};

#endif // TEXTUREDRAWER_H
