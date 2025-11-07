#pragma once
#include "vec3.h"
#include <string>
#include "IntersectionData.h"

#include "Math.h"
const float SHADOW_BIAS = 1e-3;
const float REFRACTION_BIAS = 1e-3;
const float REFRACTION_DEPTH = 5;
const double pi = 3.14159265358979323846;
const float EPSILON = 1e-6f;
class Scene;
class Ray {
public:
    Ray(const vec3& origin, const vec3& direction, const std::string& type="");
    static IntersectionData checkIntersection(const triangle* triangleArray, const int& triangleCount, const Ray& ray, const triangle& ignoreTriangle = triangle());
    static vec3 getAlbedoRay(const triangle* triangleArray, const int triangleCount, const Ray& ray, const Scene& scene, int rayDepth = 0);
    static vec3 shade(const vec3& p, const Scene& scene, const triangle& checkedTriangle, const triangle* triangleArray, const int triangleCount, const vec3& normal, const vec3& albedo);
    vec3 origin, dir;
    std::string type;
};