#include "mipmap.h"

Mipmap::Mipmap(int sizeX, int sizeY, float scaleX, float scaleY)
    : Picture(sizeX, sizeY)

    , _scaleX(scaleX)
    , _scaleY(scaleY)
{

}

float Mipmap::scaleX() const
{
    return _scaleX;
}

float Mipmap::scaleY() const
{
    return _scaleY;
}
