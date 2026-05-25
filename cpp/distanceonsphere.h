#ifndef DISTANCEONSPHERE_H
#define DISTANCEONSPHERE_H

#include "vector.h"

class Light;
class Geometry;

class DistanceOnSphere
{
public:
    DistanceOnSphere(const Light *light, float x, float y);

    float scalePointVector() const;
    void compute(Vector3f *distance);

private:
    Vector3f _pointVector;
    const Geometry *_geometry;
    float _scalePointVector;
};

#endif // DISTANCEONSPHERE_H
