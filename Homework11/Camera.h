#pragma once
#include "Matrix.h"
#include "vec3.h"

class Camera {
public:
    void setPos(const vec3& newPos);
    void setRotMatrix(const Matrix& matrix);
    vec3 getPos();
    Matrix getRotMatrix();
    void truck(const vec3& moveDir);
    void pan(const float degs);
    void tilt(const float degs);

private:
    Matrix rotationMatrix;
    vec3 position;
};