#ifndef LIGHTPLANE_H
#define LIGHTPLANE_H

class Light;
class Vector3f;

class LightPlane
{
public:
    LightPlane(const Light *light, const Vector3f &distance);


    float xScaled() const;
    float yScaled() const;

    float rScaled() const;
    float aScaled() const;


    bool insideShape();

private:
    const Light *_light;

    float _xScaled;
    float _yScaled;

    float _rScaled;
    float _aScaled;


    bool outsideRoundCorner(float rad, float ang);
};

#endif // LIGHTPLANE_H
