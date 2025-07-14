#include "Camera.h"
#include <cmath>

const float pi = 3.14159265358979323846f;

void Camera::setPos(const vec3& newPos) {
    position = newPos;
}

void Camera::setRotMatrix(const Matrix& matrix) {
    rotationMatrix = matrix;
}

vec3 Camera::getPos() {
    return position;
}

Matrix Camera::getRotMatrix() {
    return rotationMatrix;
}

void Camera::truck(const vec3& moveDir) {
    const vec3 moveDirInWorldSpace = moveDir*rotationMatrix;
    position += moveDirInWorldSpace;
}

void Camera::pan(float degs) {
    const float rads = degs * (pi/180.f);
    const Matrix rotateAroundY{
        cosf(rads), 0.f, sinf(rads),
        0.f,     1.f, 0.f,
        -sinf(rads), 0.f, cosf(rads)
    };
    rotationMatrix = rotateAroundY * rotationMatrix;
}

void Camera::tilt(float degs) {
    const float rads = degs * (pi/180.f);
    const Matrix rotateAroundX{
        1.f, 0.f, 0.f,
        0.f, cosf(rads), -sinf(rads),
        0.f, sinf(rads), cosf(rads)
    };
    rotationMatrix = rotateAroundX * rotationMatrix;
}