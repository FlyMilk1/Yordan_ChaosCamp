#include "IntersectionData.h"
IntersectionData::IntersectionData(const bool& intersects, const float& t, const vec3& p, const triangle* closestTriangle):intersects(intersects), t(t), p(p), closestTriangle(closestTriangle){}
bool IntersectionData::hasIntersection()const {
	return intersects;
}
float IntersectionData::getT()const {
	return t;
}
vec3 IntersectionData::getP()const {
	return p;
}
const triangle* IntersectionData::getClosestTriangle()const {
	return closestTriangle;
}