#pragma once

typedef struct vec3 {
    float x, y, z;
    
    vec3& operator+=(const vec3& rhs);
} vec3;

vec3 cross(const vec3& a, const vec3& b);
void normalizeVector(vec3& vector);
float length(const vec3& vector);
float dot(const vec3& a, const vec3& b);

vec3 operator*(const vec3& lhs, const float& rhs);
vec3 operator/(const vec3& lhs, const float& rhs);
vec3 operator+(const vec3& lhs, const vec3& rhs);
vec3 operator*(const vec3& lhs, const vec3& rhs);
vec3 operator-(const vec3& lhs, const vec3& rhs);
bool operator==(const vec3& a, const vec3& b);