#include "border.h"
#include "light.h"
#include "image.h"
#include "distanceonsphere.h"
#include "lightplane.h"

Border::Dr::Dr()
{

}

Border::Dr::Dr(signed char x, signed char y)
    : dx(x)
    , dy(y)
{

}

Border::Dr Border::_directions[4];

void Border::init()
{
    _directions[Up]    = Dr( 0,  1);
    _directions[Left]  = Dr(-1,  0);
    _directions[Down]  = Dr( 0, -1);
    _directions[Right] = Dr( 1,  0);
}

Border::Border(const Light *light)
    : _light(light)
    , _stateCanvas(new PixState[g_image->size()])
{

}

Border::~Border()
{
    delete[] _stateCanvas;
}

bool Border::find()
{
    if(!findStart())
        return false;

    prepare();
    exec();
    fill();
    return true;
}

const PixState *Border::stateCanvas() const
{
    return _stateCanvas;
}

bool Border::findStart()
{
    Vector2i position = _light->position();

    for(_directionFromCenter = Up; ; _directionFromCenter = Down) {
        _pix = position;
        _maxBy = _directionFromCenter == Up ? g_image->height() : -1;
        _dBy = _directionFromCenter == Up ? 1 : -1;

        for(; _pix.y() != _maxBy && computePoint(_pix) == Inside; _pix.setY(_pix.y() + _dBy))
            ;

        if(_pix.y() != _maxBy)
            break;
    }

    return (_pix.y() != position.y()) && (_pix.y() - _dBy != position.y());
}

void Border::prepare()
{
    for(size_t i = 0; i < g_image->size(); i++)
        mark(i, Undefined);

    _pix.setY(_pix.y() - _dBy);
    _begin = _pix;
    _currDir = _directionFromCenter == Up ? Left : Right;
}

void Border::exec()
{
    {
        if(_pix.y() + _dBy != _maxBy)
            mark(_pix.x(), _pix.y() + _dBy, Outside);

        mark(_begin);
    }

    for(;;) {
        const Vector2i nx = next();

        if(nx == _pix) {
            _currDir = ccw();
            continue;
        }

        const PixState nextState = computePoint(nx);
        const Vector2i nb = neighbour(nx);

        if(nextState == Inside) {
            mark(nx, nextState);

            const PixState neighbourState = computePoint(nb);
            mark(nb, neighbourState);


            if(neighbourState == Inside) {
                _pix = nb;
                _currDir = cw();
            }
            else
                _pix = nx;

            if(_pix == _begin)
                break;
        }
        else {
            mark(nb, Outside);
            mark(nx, nextState);

            _currDir = ccw();
        }
    }

    {
        const Vector2i nx = next();
        const PixState nextState = computePoint(nx);
        const Vector2i nb = neighbour(nx);

        if(nextState == Outside && _stateCanvas[g_image->coordinate(nb.x(), nb.y())] == Undefined) {
            mark(nb, Outside);
            mark(nx, Outside);
        }
    }
}

void Border::fill()
{
    Vector2i position = _light->position();


    _fillY = Inside;
    for(int i = position.y(); i < g_image->height(); i++)
        drawLine(position.x(), i);

    _fillY = Inside;
    for(int i = position.y() - 1; i >= 0; i--)
        drawLine(position.x(), i);
}

PixState Border::computePoint(Vector2i pix)
{
    Vector3f distance;
    if(!computeDistance(&distance, pix))
        return Outside;

    return LightPlane(_light, distance).insideShape() ? Inside : Outside;
}

bool Border::computeDistance(Vector3f *distance, Vector2i point) const
{
    DistanceOnSphere d(_light, point.x() + 0.5f, point.y() + 0.5f);

    if(d.scalePointVector() <= 0)
        return false;

    d.compute(distance);
    return true;
}

Vector2i Border::next()
{
    return g_image->clamp(_pix + _directions[_currDir]);
}

Vector2i Border::neighbour(Vector2i pix)
{
    return g_image->clamp(pix + _directions[cw()]);
}

unsigned char Border::cw()
{
    return (_currDir + 3) % 4;
}

unsigned char Border::ccw()
{
    return (_currDir + 1) % 4;
}

void Border::drawLine(int x, int y)
{
    const size_t indexY = g_image->coordinate(x, y);
    const PixState currentStateY = _stateCanvas[indexY];

    if(currentStateY != Undefined && currentStateY != _fillY)
        _fillY = _fillY == Inside ? Outside : Inside;


    _fill = _fillY;
    for(int i = x; i < g_image->width(); i++)
        drawPixel(i, y);

    _fill = _fillY;
    for(int i = x - 1; i >= 0; i--)
        drawPixel(i, y);
}

void Border::drawPixel(int x, int y)
{
    const size_t index = g_image->coordinate(x, y);
    const PixState currentState = _stateCanvas[index];

    _stateCanvas[index] = currentState == Undefined ? _fill : Border_;

    if(currentState != Undefined && currentState != _fill)
        _fill = _fill == Inside ? Outside : Inside;
}

void Border::mark(size_t index, PixState state)
{
    _stateCanvas[index] = state;
}

void Border::mark(int x, int y, PixState state)
{
    mark(g_image->coordinate(x, y), state);
}

void Border::mark(Vector2i vec, PixState state)
{
    mark(vec.x(), vec.y(), state);
}

void Border::mark(Vector2i begin)
{
    mark(begin, Inside);
    mark(neighbour(begin), Outside);
}


Vector2i operator+(Vector2i p, Border::Dr d)
{
    return Vector2i(p.x() + d.dx, p.y() + d.dy);
}
