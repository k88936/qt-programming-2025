#ifndef VECTOR_H
#define VECTOR_H

#include <QVector3D>
#include <iostream>
#include <cmath>

#include "box2d/math_functions.h"

class Vector : public QVector3D
{
public:
    // 构造函数
    Vector() : QVector3D{0.0f, 0.0f, 0.0f}
    {
    }

    Vector(float x, float y) : QVector3D{x, y, 0.0f}
    {
    }

    Vector(float x, float y, float z) : QVector3D{x, y, z}
    {
    }

    Vector(const b2Vec2 vec): QVector3D(vec.x, vec.y, 0.0f)
    {
    }

    Vector(const QVector3D& vec) : QVector3D(vec)
    {
    }


    Vector operator +=(const Vector& rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    Vector operator -=(const Vector& rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    operator const b2Vec2() const
    {
        return b2Vec2(x(), y());
    }

    operator const b2Rot() const
    {
        return b2Rot(x(), y());
    }

    // operator const QVector3D() const
    // {
    //     return *this;
    // }
};


inline std::ostream& operator<<(std::ostream& os, const Vector& vector)
{
    return os << "(x: " << vector.x() << ", y: " << vector.y() << ", z:" << vector.z() << ")";
}

inline Vector operator+(const Vector& lhs, const Vector& rhs)
{
    return Vector(lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z());
}

inline Vector operator-(const Vector& lhs, const Vector& rhs)
{
    return Vector(lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z());
}
#endif //VECTOR_H
