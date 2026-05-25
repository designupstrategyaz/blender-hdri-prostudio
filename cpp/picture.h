#ifndef PICTURE_H
#define PICTURE_H

#include "vector.h"

#include <stddef.h>

class Picture
{
public:
    Picture();
    Picture(int width, int height);

    virtual ~Picture();


    int width() const;
    int height() const;

    float *pixels() const;


    size_t size() const;
    size_t pixelsSize() const;

    size_t coordinate(int x, int y) const;
    Vector2i clamp(Vector2i vec) const;

protected:
    int _width;
    int _height;

    size_t _size;
    size_t _pixelsSize;

    float *_pixels;


    void init(int width, int height, const float *pixels = 0);
};

#endif // PICTURE_H
