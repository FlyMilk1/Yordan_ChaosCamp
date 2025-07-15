#include "triangle.h"
#include "vec3.h"

triangle::triangle(const vec3& v0, const vec3& v1, const vec3& v2, const Material& material) 
    : v0(v0), v1(v1), v2(v2), material(material) {
    normalVec = normal(v0, v1, v2);
}

void triangle::useBaryForColor(const float& u, const float& v) {
    material.setAlbedo({u, v, 0});
}

vec3 triangle::normal(const vec3& v0, const vec3& v1, const vec3& v2) {
    vec3 e0 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
    vec3 e1 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};
    vec3 crossVec = cross(e0, e1);
    normalizeVector(crossVec);
    return crossVec;
}