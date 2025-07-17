#pragma once
#include "triangle.h"
#include "vec3.h"
class IntersectionData {
	public:
		IntersectionData(const bool& intersects, const float& t, const vec3& p,const triangle* closestTriangle);
		bool hasIntersection()const;
		float getT()const;
		vec3 getP()const;
		const triangle* getClosestTriangle()const;
	private:
		bool intersects;
		float t;
		vec3 p;
		const triangle* closestTriangle;
};