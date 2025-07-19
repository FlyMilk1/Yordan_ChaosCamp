#pragma once
#include "vec3.h"

class Matrix {
public:
    Matrix() = default;
    Matrix(float m00, float m01, float m02,
           float m10, float m11, float m12,
           float m20, float m21, float m22);
    
    friend Matrix operator*(const Matrix& lhs, const Matrix& rhs);
    friend vec3 operator*(const vec3& lhs, const Matrix& rhs);
    
    float m[3][3];
};