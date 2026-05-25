#ifndef BORDER_H
#define BORDER_H

#include "vector.h"
#include "types.h"

#include <stddef.h>

class Light;

class Border
{
public:
    struct Dr
    {
        Dr();
        Dr(signed char x, signed char y);

        signed char dx;
        signed char dy;
    };


    static void init();


    Border(const Light *light);
    ~Border();

    bool find();
    const PixState *stateCanvas() const;

private:
    enum Direction {
          Up    = 0
        , Left  = 1
        , Down  = 2
        , Right = 3
    };


    static Dr _directions[4];


    const Light *_light;
    PixState *_stateCanvas;

    Direction _directionFromCenter;
    int _maxBy;
    int _dBy;
    Vector2i _pix;
    Vector2i _begin;
    unsigned char _currDir;

    PixState _fillY;
    PixState _fill;


    bool findStart();
    void prepare();
    void exec();
    void fill();

    PixState computePoint(Vector2i pix);
    bool computeDistance(Vector3f *distance, Vector2i point) const;

    Vector2i next();
    Vector2i neighbour(Vector2i pix);

    unsigned char cw();
    unsigned char ccw();

    void drawLine(int x, int y);
    void drawPixel(int x, int y);

    void mark(size_t index, PixState state);
    void mark(int x, int y, PixState state);
    void mark(Vector2i vec, PixState state);
    void mark(Vector2i begin);
};

Vector2i operator+(Vector2i p, Border::Dr d);

#endif // BORDER_H
