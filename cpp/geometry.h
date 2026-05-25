#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vector.h"

class Light;

class Geometry
{
public:
    Geometry(Light *light);

    const Vector3f *e() const;
    const Vector3f &lightVector() const;

    void move(Vector2i position);
    void rotate(float angle);

private:
    static const Vector3f _upVector;


    Vector3f _lightVector;
    Vector3f _e[3];

    Light *_light;
    Vector3f _lightNormal;
    Vector3f _e0[3];


    void computeLightVector();
    void computeLightNormal();
    void computeE0();

    void rotateX(Vector3f *e, float cosA, float sinA);
    void rotateY(Vector3f *e, float cosA, float sinA);

    void updateCanvas();

    void rotateESphereX(int x);
    void rotateESphereY(int y);
};

#endif // GEOMETRY_H
