#pragma once
#include "vec3.h"
#include <string>
#include "IntersectionData.h"
#include <vector>
#include "Math.h"
const float SHADOW_BIAS = 1e-3f;
const float REFRACTION_BIAS = 1e-3f;
const float REFRACTION_DEPTH = 5;
const double pi = 3.14159265358979323846;
const float EPSILON = 1e-3f;
class Scene;
class Ray {
public:
    Ray(const vec3& origin, const vec3& direction, const std::string& type="");
    static IntersectionData checkIntersection(const std::vector<triangle>& triangleArray, const Ray& ray, const triangle& ignoreTriangle = triangle());
    static vec3 getAlbedoRay(const std::vector<triangle>& triangleArray, const Ray& ray, const Scene& scene, const int& rayDepth = 0);
    static vec3 shade(const vec3& p, const Scene& scene, const triangle& checkedTriangle, const std::vector<triangle>& triangleArray, const vec3& normal, const vec3& albedo);
    vec3 origin, dir;
    std::string type;
};