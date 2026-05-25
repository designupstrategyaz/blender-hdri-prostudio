#include "texturedrawer.h"
#include "light.h"
#include "math_utils.h"
#include "image.h"
#include "lightplane.h"
#include "mipmap.h"

using namespace std;

TextureDrawer::TextureDrawer()
    : _mp2(log2i(g_image->height()))
    , _maxValue(pow2(_mp2))
{

}

void TextureDrawer::setLight(const Light *light)
{
    _light = light;
}

int TextureDrawer::mp2() const
{
    return _mp2;
}

int TextureDrawer::maxValue() const
{
    return _maxValue;
}

void TextureDrawer::point(const float distanceScale, const LightPlane &l, float *color)
{
    float mipmapDistanceX = 0;
    float mipmapDistanceY = 0;

    int cmpX = 0;
    int cmpY = 0;

    {
        const float scaleFactorY = _light->height() / M_PI_F / distanceScale;
        int i;
        for(i = _mp2; i >= 0; i--) {
            if(sc(i) <= scaleFactorY) {
                cmpY = i;
                break;
            }
        }
        if(i == -1)
            cmpY = 0;

        if(cmpY == _mp2)
            mipmapDistanceY = 0;
        else {
            const float scaleCmpY = sc(cmpY);
            mipmapDistanceY = (scaleFactorY - scaleCmpY) / (sc(cmpY + 1) - scaleCmpY);
        }
    }
    {
        const float scaleFactorX = _light->width() / M_PI_F / distanceScale;
        int i;
        for(i = _mp2; i >= 0; i--) {
            if(sc(i) <= scaleFactorX) {
                cmpX = i;
                break;
            }
        }
        if(i == -1)
            cmpX = 0;

        if(cmpX == _mp2)
            mipmapDistanceX = 0;
        else {
            const float scaleCmpX = sc(cmpX);
            mipmapDistanceX = (scaleFactorX - scaleCmpX) / (sc(cmpX + 1) - scaleCmpX);
        }
    }




    const Vector2f inR = Vector2f(l.xScaled(), l.yScaled()) / 2.f + Vector2f(0.5, 0.5);
    const vector<vector<Mipmap*> > &mm = _light->mipmaps();

    if(cmpY == _mp2 && cmpX == _mp2)
        bilinearFiltering(color, inR, mm[_mp2][_mp2]);
    else if(cmpY == _mp2)
        trilinearFiltering(color, inR, mm[_mp2][cmpX], mm[_mp2][cmpX + 1], mipmapDistanceX);
    else if(cmpX == _mp2)
        trilinearFiltering(color, inR, mm[cmpY][_mp2], mm[cmpY + 1][_mp2], mipmapDistanceY);
    else
        anisotropicFiltering(color, inR, mm[cmpY][cmpX], mm[cmpY][cmpX + 1], mm[cmpY + 1][cmpX], mm[cmpY + 1][cmpX + 1], mipmapDistanceX, mipmapDistanceY);
}

#define RP(X, Y) (&map->pixels()[map->coordinate(X, Y) * 4])

void TextureDrawer::bilinearFiltering(float *outPixel, Vector2f inR, const Mipmap *map)
{
    const Vector2f r = inR * (Vector2f(map->width(), map->height()) - Vector2f(1, 1));
    const Vector2i pr(r);
    const Vector2f dr = r - Vector2f(pr);

    for(int j = 0; j < 4; j++)
        outPixel[j] = RP(pr.x(), pr.y())[j] + (RP(pr.x() + 1, pr.y())[j] - RP(pr.x(), pr.y())[j]) * dr.x() + (RP(pr.x(), pr.y() + 1)[j] - RP(pr.x(), pr.y())[j]) * dr.y() + (RP(pr.x() + 1, pr.y() + 1)[j] - RP(pr.x(), pr.y() + 1)[j] - RP(pr.x() + 1, pr.y())[j] + RP(pr.x(), pr.y())[j]) * dr.x() * dr.y();
}

void TextureDrawer::trilinearFiltering(float *outPixel, Vector2f inR, const Mipmap *smallerMap, const Mipmap *biggerMap, float mipmapDistance)
{
    float smaller[4];
    bilinearFiltering(smaller, inR, smallerMap);

    float bigger[4];
    bilinearFiltering(bigger, inR, biggerMap);


    for(int i = 0; i < 4; i++)
        outPixel[i] = smaller[i] * (1 - mipmapDistance) + bigger[i] * mipmapDistance;
}

void TextureDrawer::anisotropicFiltering(float *outPixel, Vector2f inR, const Mipmap *yx, const Mipmap *yX, const Mipmap *Yx, const Mipmap *YX, float mipmapDistanceX, float mipmapDistanceY)
{
    float smallerY[4];
    trilinearFiltering(smallerY, inR, yx, yX, mipmapDistanceX);

    float biggerY[4];
    trilinearFiltering(biggerY, inR, Yx, YX, mipmapDistanceX);


    for(int i = 0; i < 4; i++)
        outPixel[i] = smallerY[i] * (1 - mipmapDistanceY) + biggerY[i] * mipmapDistanceY;
}

float TextureDrawer::sc(int r)
{
    return _light->mipmaps()[0][r]->scaleX();
}
