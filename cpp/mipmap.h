#ifndef MIPMAP_H
#define MIPMAP_H

#include "picture.h"

class Mipmap : public Picture
{
public:
    Mipmap(int sizeX, int sizeY, int scaleX, int scaleY);

    float scaleX() const;
    float scaleY() const;

private:
    float _scaleX;
    float _scaleY;
};

#endif // MIPMAP_H
