#include "lightplane.h"
#include "vector.h"
#include "geometry.h"
#include "math_utils.h"
#include "light.h"

LightPlane::LightPlane(const Light *light, const Vector3f &distance)
    : _light(light)
{
    const Geometry *geometry = _light->geometry();

    const float xReal = Vector3f::dot(distance, geometry->e()[0]);
    const float yReal = Vector3f::dot(distance, geometry->e()[1]);

    _xScaled = xReal / tanf(_light->width() / 2);
    _yScaled = yReal / tanf(_light->height() / 2);

    _rScaled = sqrtf(_xScaled * _xScaled + _yScaled * _yScaled);
}

float LightPlane::xScaled() const
{
    return _xScaled;
}

float LightPlane::yScaled() const
{
    return _yScaled;
}

float LightPlane::rScaled() const
{
    return _rScaled;
}

float LightPlane::aScaled() const
{
    return _aScaled;
}

bool LightPlane::insideShape()
{
    if(_rScaled > 1)
        return false;

    const float cosAScaled = _rScaled == 0 ? 0 : _xScaled / _rScaled;
    _aScaled = arccosf(cosAScaled) * sign(_yScaled);

    if(!_light->isEllipse()) {
        const float f = 2 * M_PI_F / _light->sides();
        float aInTriangle = _aScaled;

        while(aInTriangle < -f / 2)
            aInTriangle += f;
        while(aInTriangle >= f / 2)
            aInTriangle -= f;

        if(outsideRoundCorner(_rScaled, f / 2 + aInTriangle) || outsideRoundCorner(_rScaled, f / 2 - aInTriangle))
            return false;

        if(_rScaled * cosf(aInTriangle) / cosf(f / 2) > 1)
            return false;
    }

    return true;
}

bool LightPlane::outsideRoundCorner(float rad, float ang)
{
    const float f = 2 * M_PI_F / _light->sides();

    const float xScaledRotated = rad * cosf(ang);
    const float yScaledRotated = rad * sinf(ang);

    const float xCorner = xScaledRotated - (1 - _light->cornerRadius() / cosf(f / 2));
    const float yCorner = yScaledRotated;

    const float rCorner = sqrtf(xCorner * xCorner + yCorner * yCorner);
    if(rCorner > _light->cornerRadius()) {
        const float aCorner = arccosf(xCorner / rCorner);
        if(aCorner <= f / 2)
            return true;
    }

    return false;
}
