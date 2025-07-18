#pragma once
#include "vec3.h"
#include "triangle.h"
#include <vector>
#include "Mesh.h"
#include <algorithm>
#include "Ray.h"
class AABB {
public:
	void generateAABB(std::vector<Mesh> meshArray);
	bool checkSides(const Ray ray, float& tHit)const;
private:
	vec3 minPoint = { FLT_MAX, FLT_MAX, FLT_MAX };
	vec3 maxPoint = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	vec3 checkMin(const vec3& point0, const vec3& point1);
	vec3 checkMax(const vec3& point0, const vec3& point1);
	vec3 arrayToVec3(const float* array)const;
	void vec3ToArray(float* array, const vec3& vec)const;
	
	
};