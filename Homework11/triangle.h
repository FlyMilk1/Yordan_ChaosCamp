#pragma once
#include "vec3.h"
#include "Material.h"

class triangle {
public:
    triangle(const vec3& v0 = {0,0,0}, const vec3& v1 = {0,0,0}, 
        const vec3& v2 = {0,0,0}, const vec3& uv0 = { 0,0,0 }, const vec3& uv1 = { 0,0,0 }, const vec3& uv2 = { 0,0,0 }, const Material& material = Material());
    vec3 v0, v1, v2;
    Material material;
    vec3 normalVec;
    vec3 v0N, v1N, v2N;
    vec3 uv0, uv1, uv2;
    //DEPRECATED
    //void useBaryForColor(const float& u, const float& v);

private:
    vec3 normal(const vec3& v0, const vec3& v1, const vec3& v2);
};