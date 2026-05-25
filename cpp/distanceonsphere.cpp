#include "distanceonsphere.h"
#include "math_utils.h"
#include "image.h"
#include "vector.h"
#include "light.h"
#include "geometry.h"

DistanceOnSphere::DistanceOnSphere(const Light *light, float x, float y)
{
    const float f = 2 * M_PI_F * (1 - x / g_image->width());
    const float t = M_PI_F * (y / g_image->height() - 0.5f);
    const float cosT = cosf(t);

    _pointVector = new Vector3f(cosT * cosf(f), cosT * sinf(f), sinf(t));
    _geometry = light->geometry();
    _scalePointVector = 1 / Vector3f::dot(*_pointVector, _geometry->lightVector());
}

DistanceOnSphere::~DistanceOnSphere()
{
    delete _pointVector;
}

float DistanceOnSphere::scalePointVector() const
{
    return _scalePointVector;
}

void DistanceOnSphere::compute(Vector3f *distance)
{
    *distance = (_scalePointVector * (*_pointVector) - _geometry->lightVector());
}
