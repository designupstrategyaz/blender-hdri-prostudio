#include "vector.h"
#include "image.h"
#include "math_utils.h"

float Vector3f::dot(const Vector3f &a, const Vector3f &b)
{
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

Vector3f Vector3f::cross(const Vector3f &a, const Vector3f &b)
{
    return {a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(), a.x() * b.y() - a.y() * b.x()};
}

Vector3f::Vector3f()
{

}

Vector3f::Vector3f(float x, float y, float z)
    : _x(x)
    , _y(y)
    , _z(z)
{

}

float Vector3f::x() const
{
    return _x;
}

float Vector3f::y() const
{
    return _y;
}

float Vector3f::z() const
{
    return _z;
}

void Vector3f::setX(float x)
{
    _x = x;
}

void Vector3f::setY(float y)
{
    _y = y;
}

void Vector3f::setZ(float z)
{
    _z = z;
}

Vector3f Vector3f::normalized() const
{
    return (*this) / length();
}

float Vector3f::length() const
{
    return sqrt(_x * _x + _y * _y + _z * _z);
}

Vector3f Vector3f::operator-() const
{
    return Vector3f(-_x, -_y, -_z);
}

Vector3f operator*(float a, const Vector3f &b)
{
    return Vector3f(a * b._x, a * b._y, a * b._z);
}

Vector3f operator/(const Vector3f &a, float b)
{
    return Vector3f(a._x / b, a._y / b, a._z / b);
}

Vector3f operator+(const Vector3f &a, const Vector3f &b)
{
    return Vector3f(a._x + b._x, a._y + b._y, a._z + b._z);
}

Vector3f operator-(const Vector3f &a, const Vector3f &b)
{
    return Vector3f(a._x - b._x, a._y - b._y, a._z - b._z);
}


Matrix3x3f::Matrix3x3f(float angle, const Vector3f &axis)
{
    const float cosA = cos(angle);
    const float sinA = sin(angle);

    const float a12 = axis.x() * axis.y() * cosA;
    const float a23 = axis.y() * axis.z() * cosA;
    const float a31 = axis.z() * axis.x() * cosA;

    const float b1 = axis.x() * sinA;
    const float b2 = axis.y() * sinA;
    const float b3 = axis.z() * sinA;

    _data = new float[9] {
          (1 - axis.x() * axis.x()) * cosA
        , -a12 - b3
        , -a31 + b2

        , -a12 + b3
        , (1 - axis.y() * axis.y()) * cosA
        , -a23 - b1

        , -a31 - b2
        , -a23 + b1
        , (1 - axis.z() * axis.z()) * cosA
    };
}

Matrix3x3f::~Matrix3x3f()
{
    delete[] _data;
}

Vector3f operator*(const Matrix3x3f &m, const Vector3f &v)
{
    return Vector3f(
          m[0][0] * v.x() + m[0][1] * v.y() + m[0][2] * v.z()
        , m[1][0] * v.x() + m[1][1] * v.y() + m[1][2] * v.z()
        , m[2][0] * v.x() + m[2][1] * v.y() + m[2][2] * v.z()
    );
}

float *Matrix3x3f::operator[](int index) const
{
    return &_data[index * 3];
}
