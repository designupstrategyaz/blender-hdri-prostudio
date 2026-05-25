#ifndef VECTOR_H
#define VECTOR_H

template<class T>
class Vector2
{
public:
    Vector2()
    {

    }
    Vector2(T x, T y)
        : _x(x)
        , _y(y)
    {

    }
    Vector2(const Vector2 &vector)
        : _x(vector._x)
        , _y(vector._y)
    {

    }

    template<class T2>
    Vector2(Vector2<T2> vector)
        : _x(vector.x())
        , _y(vector.y())
    {

    }

    T x() const
    {
        return _x;
    }
    T y() const
    {
        return _y;
    }

    void setX(T x)
    {
        _x = x;
    }
    void setY(T y)
    {
        _y = y;
    }


    friend Vector2 operator/(const Vector2 &a, float b)
    {
        return Vector2<T>(a._x / b, a._y / b);
    }
    friend Vector2 operator*(const Vector2 &a, const Vector2 &b)
    {
        return Vector2<T>(a._x * b._x, a._y * b._y);
    }

    friend Vector2 operator+(const Vector2 &a, const Vector2 &b)
    {
        return Vector2<T>(a._x + b._x, a._y + b._y);
    }
    friend Vector2 operator-(const Vector2 &a, const Vector2 &b)
    {
        return Vector2<T>(a._x - b._x, a._y - b._y);
    }

    friend bool operator==(Vector2 a, Vector2 b)
    {
        return a._x == b._x && a._y == b._y;
    }

private:
    T _x;
    T _y;
};

template class Vector2<int>;
template class Vector2<float>;

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;


class Vector3f
{
public:
    static float dot(const Vector3f &a, const Vector3f &b);
    static Vector3f cross(const Vector3f &a, const Vector3f &b);


    Vector3f();
    Vector3f(float x, float y, float z);

    float x() const;
    float y() const;
    float z() const;

    void setX(float x);
    void setY(float y);
    void setZ(float z);

    Vector3f normalized() const;
    float length() const;


    Vector3f operator-() const;

    friend Vector3f operator*(float a, const Vector3f &b);
    friend Vector3f operator/(const Vector3f &a, float b);

    friend Vector3f operator+(const Vector3f &a, const Vector3f &b);
    friend Vector3f operator-(const Vector3f &a, const Vector3f &b);

private:
    float _x;
    float _y;
    float _z;
};

class Matrix3x3f
{
public:
    Matrix3x3f(float angle, const Vector3f &axis);
    ~Matrix3x3f();

    friend Vector3f operator*(const Matrix3x3f &m, const Vector3f &v);

private:
    float *_data;


    float *operator[](int index) const;
};

#endif // VECTOR_H
