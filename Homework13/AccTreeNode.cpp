#include "AccTreeNode.h"
AccTreeNode::AccTreeNode(const AABB& aabb, const int leftNodeIdx, const int rightNodeIdx) :aabb(aabb){ children[0] = leftNodeIdx; children[1] = rightNodeIdx; }



IntersectionData AccTreeNode::intersect(const Ray& ray) const
{
	return Ray::checkIntersection(triangles, ray);
}


