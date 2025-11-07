#pragma once
#include "AABB.h"
#include "Ray.h"
const int maxDepth = 16;
const int maxTriangles = 2;
struct AccTreeNode {
	AccTreeNode(const AABB& aabb, const int leftNodeIdx, const int rightNodeIdx);
	
	IntersectionData intersect(const Ray& ray) const;
	
	std::vector<triangle> triangles;
	AABB aabb;
	int children[2];
	int parent;
	int depth=0;
};