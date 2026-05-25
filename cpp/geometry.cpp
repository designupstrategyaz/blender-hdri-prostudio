#include "geometry.h"
#include "vector.h"
#include "light.h"
#include "math_utils.h"
#include "image.h"

const Vector3f Geometry::_upVector(0, 0, 1);

Geometry::Geometry(Light *light)
    : _light(light)
{
    computeLightVector();
    updateCanvas();

    for(int i = 0; i < 3; i++)
        _e[i] = _e0[i];
}

const Vector3f *Geometry::e() const
{
    return _e;
}

const Vector3f &Geometry::lightVector() const
{
    return _lightVector;
}

void Geometry::move(Vector2i position)
{
    const Vector2i oldPosition = _light->position();
    _light->setPosition(position);
    computeLightVector();

    rotateESphereY(oldPosition.y());
    rotateESphereX(oldPosition.x());

    updateCanvas();
}

void Geometry::rotate(float angle)
{
    const float da = angle - _light->angle();
    Matrix3x3f rotation(da, _e0[2]);

    for(int i = 0; i < 3; i++)
        _e[i] = rotation * _e[i];
}

void Geometry::computeLightVector()
{
    const float x = _light->position().x() + 0.5f;
    const float y = _light->position().y() + 0.5f;

    const float f = 2 * M_PI_F * (1 - x / g_image->width());
    const float t = M_PI_F * (y / g_image->height() - 0.5f);
    const float cosT = cosf(t);

    _lightVector.setX(cosT * cosf(f));
    _lightVector.setY(cosT * sinf(f));
    _lightVector.setZ(sinf(t));
}

void Geometry::computeLightNormal()
{
    _lightNormal = Vector3f::cross(_lightVector, _upVector).normalized();
}

void Geometry::computeE0()
{
    _e0[0] = _lightNormal;
    _e0[1] = Vector3f::cross(_e0[0], _lightVector);
    _e0[2] = -_lightVector;
}

void Geometry::rotateX(Vector3f *e, float cosA, float sinA)
{
    const float ei0 = e->x();
    const float ei1 = e->y();

    e->setX(ei0 * cosA - ei1 * sinA);
    e->setY(ei0 * sinA + ei1 * cosA);
}

void Geometry::rotateY(Vector3f *e, float cosA, float sinA)
{
    const float ei0 = Vector3f::dot(*e, -_e0[2]);
    const float ei1 = Vector3f::dot(*e, _e0[1]);

    *e = (ei0 * cosA - ei1 * sinA) * (-_e0[2]) + (ei0 * sinA + ei1 * cosA) * _e0[1] + Vector3f::dot(*e, _e0[0]) * _e0[0];
}

void Geometry::updateCanvas()
{
    computeLightNormal();
    computeE0();
}

void Geometry::rotateESphereX(int x)
{
    const float da = -2 * M_PI_F * (_light->position().x() - x) / g_image->width();

    const float cosA = cosf(da);
    const float sinA = sinf(da);


    for(int i = 0; i < 3; i++)
        rotateX(&_e[i], cosA, sinA);
}

void Geometry::rotateESphereY(int y)
{
    const float da = M_PI_F * (_light->position().y() - y) / g_image->height();

    const float cosA = cosf(da);
    const float sinA = sinf(da);


    for(int i = 0; i < 3; i++)
        rotateY(&_e[i], cosA, sinA);
}

