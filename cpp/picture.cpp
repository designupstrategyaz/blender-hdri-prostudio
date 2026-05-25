#include "picture.h"

Picture::Picture()
    : _pixels(0)
{

}

Picture::Picture(int width, int height)
{
    init(width, height);
}

Picture::~Picture()
{
    delete[] _pixels;
}

int Picture::width() const
{
    return _width;
}

int Picture::height() const
{
    return _height;
}

float *Picture::pixels() const
{
    return _pixels;
}

size_t Picture::size() const
{
    return _size;
}

size_t Picture::pixelsSize() const
{
    return _pixelsSize;
}

size_t Picture::coordinate(int x, int y) const
{
    return y * _width + x % _width;
}

Vector2i Picture::clamp(Vector2i vec) const
{
    while(vec.x() < 0)
        vec.setX(vec.x() + _width);
    while(vec.x() >= _width)
        vec.setX(vec.x() - _width);

    if(vec.y() < 0)
        vec.setY(0);
    if(vec.y() >= _height)
        vec.setY(_height - 1);

    return vec;
}

void Picture::init(int width, int height, const float *pixels)
{
    _width = width;
    _height = height;

    _size = _width * _height;
    _pixelsSize = _size * 4;

    _pixels = new float[_pixelsSize];

    if(pixels) {
        for(size_t i = 0; i < _pixelsSize; i++)
            _pixels[i] = pixels[i];
    }
}
