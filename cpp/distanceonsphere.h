#ifndef DISTANCEONSPHERE_H
#define DISTANCEONSPHERE_H

class Picture;
class Light;
class Vector3f;
class Geometry;

class DistanceOnSphere
{
public:
    DistanceOnSphere(const Light *light, float x, float y);
    ~DistanceOnSphere();

    float scalePointVector() const;
    void compute(Vector3f *distance);

private:
    Vector3f *_pointVector;
    const Geometry *_geometry;
    float _scalePointVector;
};

#endif // DISTANCEONSPHERE_H
