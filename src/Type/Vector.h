#ifndef VECTOR_H
#define VECTOR_H

#include <QVector3D>
#include "box2d/math_functions.h"

class Vector : public b2Vec2 {
public:
    // 构造函数
    Vector() : b2Vec2{0.0f, 0.0f} {
    }

    Vector(float x, float y) : b2Vec2{x, y} {
    }

    Vector(const b2Vec2 &vec): b2Vec2(vec) {
    };

    Vector(const b2Rot &rot): b2Vec2(rot.c, rot.s) {
    };

    // 获取向量长度
    float Length() const {
        return b2Length(*this);
    }

    // 归一化向量
    Vector Normalize() const {
        return b2Normalize(*this);
    }

    // 向量加法
    Vector Add(const Vector &other) const {
        return b2Add(*this, other);
    }

    // 向量减法
    Vector Sub(const Vector &other) const {
        return b2Sub(*this, other);
    }

    // 标量乘法（成员函数）
    Vector Scale(float scalar) const {
        return b2MulSV(scalar, *this);
    }

    // 点积
    float Dot(const Vector &other) const {
        return b2Dot(*this, other);
    }

    // 叉积
    float Cross(const Vector &other) const {
        return b2Cross(*this, other);
    }

    // 静态函数用于标量乘法（交换律）
    static Vector Multiply(float scalar, const Vector &vec) {
        return vec.Scale(scalar);
    }

    // 向量夹角
    float AngleBetween(const Vector &other) const {
        Vector a = this->Normalize();
        Vector b = other.Normalize();
        return acosf(a.Dot(b));
    }

    // 判断是否为零向量
    bool IsZero() const {
        return x == 0.0f && y == 0.0f;
    }

    // 是否有效
    bool IsValid() const {
        return b2IsValidVec2(*this);
    }

    operator QVector3D() {
        constexpr float z = 0.0f;
        return {x, y, z};
    }
};

inline std::ostream &operator<<(std::ostream &os, const Vector &vector) {
    return os << "(x: " << vector.x << ", y: " << vector.y << ")";
}
#endif //VECTOR_H
