#pragma once
#include "AABB.h"
#include "Ray.h"
const int maxDepth = 5;
const int maxTriangles = 1;
struct AccTreeNode {
	AccTreeNode(const AABB& aabb, const int leftNodeIdx, const int rightNodeIdx);
	
	IntersectionData intersect(const Ray& ray) const;
	
	std::vector<triangle> triangles;
	AABB aabb;
	int children[2];
	int parent;
	int depth=0;
};