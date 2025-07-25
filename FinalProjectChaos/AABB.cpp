#include "AABB.h"
void AABB::generateAABB(std::vector<Mesh> meshArray)
{
	std::vector<triangle> triangles;
	for (int meshIdx=0; meshIdx < meshArray.size(); meshIdx++) {
		meshArray[meshIdx].addTriangles(triangles);
	}
	for (triangle& tr : triangles) {
		minPoint = checkMin(minPoint, tr.v0);
		minPoint = checkMin(minPoint, tr.v1);
		minPoint = checkMin(minPoint, tr.v2);

		maxPoint = checkMax(maxPoint, tr.v0);
		maxPoint = checkMax(maxPoint, tr.v1);
		maxPoint = checkMax(maxPoint, tr.v2);
	}
}
void AABB::generateAABBTriangle(const triangle& tr)
{
	minPoint = checkMin(minPoint, tr.v0);
	minPoint = checkMin(minPoint, tr.v1);
	minPoint = checkMin(minPoint, tr.v2);

	maxPoint = checkMax(maxPoint, tr.v0);
	maxPoint = checkMax(maxPoint, tr.v1);
	maxPoint = checkMax(maxPoint, tr.v2);
}
vec3 AABB::arrayToVec3(const float* array)const {
		vec3 vec = { array[0], array[1], array[2] };
		return vec;
}
void AABB::vec3ToArray(float* array, const vec3& vec)const{
		array[0] = vec.x;
		array[1] = vec.y;
		array[2] = vec.z;
}
bool AABB::checkSides(const Ray ray, float& tHit)const
{
	float tmin = -FLT_MAX;
	float tmax = FLT_MAX;
	float minBox[3];
	float maxBox[3];
	float origin[3];
	float dir[3];
	vec3ToArray(minBox, minPoint);
	vec3ToArray(maxBox, maxPoint);
	vec3ToArray(origin, ray.origin);
	vec3ToArray(dir, ray.dir);
	for (int i = 0; i < 3; ++i) {
		float invD = (fabs(dir[i]) > EPSILON) ? 1.0f / dir[i] : EPSILON;
		float t0 = (minBox[i] - origin[i]) * invD;
		float t1 = (maxBox[i] - origin[i]) * invD;
		if (invD < 0.0f) std::swap(t0, t1);
		tmin = std::max(tmin, t0);
		tmax = std::min(tmax, t1);
		if (tmax < tmin)
			return false;
	}

	tHit = tmin;
	return true;

}

void AABB::splitAABB(AABB& aabb0, AABB& aabb1, const int& axis)
{
	float max[3];
	float min[3];
	vec3ToArray(max, maxPoint);
	vec3ToArray(min, minPoint);
	float mid = (max[axis] - min[axis]) / 2;
	float splitPlaneCoordinate = min[axis] + mid;
	AABB A=*this, B=*this;
	max[axis] = splitPlaneCoordinate;
	min[axis] = splitPlaneCoordinate;
	vec3 aMax = arrayToVec3(max);
	vec3 bMin = arrayToVec3(min);
	A.maxPoint = aMax;
	B.minPoint = bMin;
	aabb0 = A;
	aabb1 = B;
}

bool AABB::checkIntersection(const AABB& aabb)
{
	float triMin[3], triMax[3], nodeMin[3], nodeMax[3];
	vec3ToArray(triMin, aabb.minPoint);
	vec3ToArray(triMax, aabb.maxPoint);
	vec3ToArray(nodeMin, minPoint);
	vec3ToArray(nodeMax, maxPoint);
	for (int axis = 0; axis < 3; axis++) {
		if (triMin[axis] > nodeMax[axis])return false;
		if (triMax[axis] < nodeMin[axis])return false;
	}
	return true;
}

vec3 AABB::checkMin(const vec3& point0, const vec3& point1)
{
	vec3 min;
	min.x = std::min(point0.x, point1.x);
	min.y = std::min(point0.y, point1.y);
	min.z = std::min(point0.z, point1.z);
	return min;
}

vec3 AABB::checkMax(const vec3& point0, const vec3& point1)
{
	vec3 max;
	max.x = std::max(point0.x, point1.x);
	max.y = std::max(point0.y, point1.y);
	max.z = std::max(point0.z, point1.z);
	return max;
}
