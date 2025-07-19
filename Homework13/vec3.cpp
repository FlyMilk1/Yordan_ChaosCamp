#include "vec3.h"
#include <cmath>
#include <limits>

vec3& vec3::operator+=(const vec3& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

vec3 cross(const vec3& a, const vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

void normalizeVector(vec3& vector) {
    float sqrtVector = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    vector = { vector.x / sqrtVector, vector.y / sqrtVector, vector.z / sqrtVector };
}

float length(const vec3& vector) {
    return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

float dot(const vec3& a, const vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 operator*(const vec3& lhs, const float& rhs) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
}

vec3 operator/(const vec3& lhs, const float& rhs) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
}

vec3 operator+(const vec3& lhs, const vec3& rhs) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}

vec3 operator*(const vec3& lhs, const vec3& rhs) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
}

vec3 operator-(const vec3& lhs, const vec3& rhs) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}

bool operator==(const vec3& a, const vec3& b) {
    const float EPSILON = std::numeric_limits<float>::epsilon();
    return std::fabs(a.x - b.x) < EPSILON &&
           std::fabs(a.y - b.y) < EPSILON &&
           std::fabs(a.z - b.z) < EPSILON;
}