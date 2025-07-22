#include "Matrix.h"

Matrix::Matrix(float m00, float m01, float m02,
               float m10, float m11, float m12,
               float m20, float m21, float m22) {
    m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
    m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
    m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
}

Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    Matrix result;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            result.m[row][col] = 0.0f;
            for (int k = 0; k < 3; ++k) {
                result.m[row][col] += lhs.m[row][k] * rhs.m[k][col];
            }
        }
    }
    return result;
}

vec3 operator*(const vec3& vec,const Matrix& mat) {
    return {
        mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z,
        mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z,
        mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z
    };
}