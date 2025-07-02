//
// Created by kvtoD on 6/30/2025.
//

#ifndef MATRIX_H
#define MATRIX_H
#include <qmatrix4x4.h>

#include "Vector.h"
#include "box2d/math_functions.h"


class Matrix : public b2Transform {
public:
    // 构造函数
    Matrix() : b2Transform{{0.0f, 0.0f}, {1.0f, 0.0f}} {
    } // 默认单位变换
    Matrix(const b2Vec2 &position, const b2Rot &rotation) : b2Transform{position, rotation} {
    }

    Matrix(const b2Transform &t) : b2Transform{t.p, t.q} {
    }


    // 应用变换到点
    Vector TransformPoint(const Vector &point) const {
        return b2TransformPoint(*this, point);
    }

    // 逆变换点
    Vector InverseTransformPoint(const Vector &point) const {
        return b2InvTransformPoint(*this, point);
    }

    // 组合变换（A * B）
    Matrix Multiply(const Matrix &other) const {
        return b2MulTransforms(*this, other);
    }

    // 获取位置
    Vector GetPosition() const {
        return p;
    }

    // 获取旋转
    Vector GetRotation() const {
        return q;
    }

    // 设置位置
    void SetPosition(const Vector &position) {
        p = position;
    }

    // 设置旋转
    void SetRotation(float radians) {
        q = b2MakeRot(radians);
    }

    // 静态函数用于创建平移矩阵
    static Matrix MakeTranslation(const Vector &translation) {
        return Matrix(translation, b2Rot_identity);
    }

    // 静态函数用于创建旋转矩阵
    static Matrix MakeRotation(float radians) {
        return Matrix(b2Vec2_zero, b2MakeRot(radians));
    }

    // 是否有效
    bool IsValid() const {
        return b2IsValidTransform(*this);
    }

    operator QMatrix4x4() const {
        constexpr float z = 0.0f;
        float x = p.x;
        float y = p.y;
        float c = q.c;
        float s = q.s;

        return {
            c, s, 0, x,
            -s, c, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
        };
    }
};

inline std::ostream &operator <<(std::ostream &os, const Matrix &matrix) {
    return os << "(position: " << matrix.GetPosition() << ", rotation: " << matrix.GetRotation() << ")";
}

#endif //MATRIX_H
