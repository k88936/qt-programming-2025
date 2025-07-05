//
// Created by kvtoD on 6/30/2025.
//

#ifndef MATRIX_H
#define MATRIX_H
#include <qmatrix4x4.h>

#include "Vector.h"
#include "box2d/math_functions.h"


class Matrix : public QMatrix4x4
{
public:
    float flip = 1;
    // 构造函数
    Matrix()
    {
    } // 默认单位矩阵

    Matrix(const QMatrix4x4& matrix) : QMatrix4x4{matrix}
    {
    }

    explicit Matrix(const b2Transform& transform): QMatrix4x4{
        transform.q.c, -transform.q.s, 0, transform.p.x,
        transform.q.s, transform.q.c, 0, transform.p.y,
        0, 0, 1, 0,
        0, 0, 0, 1
    }
    {
    }

    void updateFlip(const float flip)
    {
        this->flip = flip;
    }

    void updateTransform(const b2Transform& transform)
    {
        (*this)(0, 0) = flip * transform.q.c;
        (*this)(0, 1) = flip * transform.q.s;
        (*this)(0, 2) = 0;
        (*this)(0, 3) = transform.p.x;

        (*this)(1, 0) = -transform.q.s;
        (*this)(1, 1) = transform.q.c;
        (*this)(1, 2) = 0;
        (*this)(1, 3) = transform.p.y;

        (*this)(2, 0) = 0;
        (*this)(2, 1) = 0;
        (*this)(2, 2) = 1;
        (*this)(2, 3) = 0;

        (*this)(3, 0) = 0;
        (*this)(3, 1) = 0;
        (*this)(3, 2) = 0;
        (*this)(3, 3) = 1;
    }


    // 获取位置
    Vector getPosition() const
    {
        return Vector(column(3).x(), column(3).y(), column(3).z());
    }

    // 获取旋转（提取Z轴旋转角度）
    Vector getRotation() const
    {
        return Vector((*this)(0, 0), (*this)(1, 0));
    }

    operator const b2Transform() const
    {
        b2Transform result;
        result.p.x = column(3).x();
        result.p.y = column(3).y();
        result.q.c = (*this)(0, 0);
        result.q.s = (*this)(1, 0);
        return result;
    }
};

inline std::ostream& operator <<(std::ostream& os, const Matrix& matrix)
{
    return os << "(position: " << matrix.getPosition() << ", rotation: " << matrix.getRotation() << ")";
}

#endif //MATRIX_H
