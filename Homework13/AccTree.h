#pragma once
#include "AccTreeNode.h"
#include <stack>
struct AccTree {
	std::vector<AccTreeNode> accTree;
	int addNode(const AABB& aabb, const int& child0Idx, const int& child1Idx/*, const std::vector<triangle>& triangles*/)
	{
		AccTreeNode node(aabb, child0Idx, child1Idx/*, triangles*/);
		accTree.push_back(node);
		return accTree.size() - 1;
		
	}
	void buildAccTree(const int& parentIdx, const int& depth, const std::vector<triangle>& triangles)
	{
		AABB childAABB0, childAABB1;
		int child0Idx, child1Idx;
		std::vector<triangle> child0Triangles, child1Triangles;
		if (depth >= maxDepth || triangles.size() <= maxTriangles) {
			accTree[parentIdx].triangles = triangles;
			return;
		}

		accTree[parentIdx].aabb.splitAABB(childAABB0, childAABB1, depth % 3);
	
		for (int triangleIdx = 0; triangleIdx < triangles.size(); triangleIdx++) {
			AABB triAABB;
			triAABB.generateAABBTriangle(triangles[triangleIdx]);
			if (triAABB.checkIntersection(childAABB0)) {
				child0Triangles.push_back(triangles[triangleIdx]);
			}
			if (triAABB.checkIntersection(childAABB1)) {
				child1Triangles.push_back(triangles[triangleIdx]);
			}
		}
		if (child0Triangles.size() > 0) {
			child0Idx = addNode(childAABB0, -1, -1);
			accTree[parentIdx].children[0] = child0Idx;
			buildAccTree(child0Idx, depth + 1, child0Triangles);
		}
		if (child1Triangles.size() > 0) {
			child1Idx = addNode(childAABB1, -1, -1);
			accTree[parentIdx].children[1] = child1Idx;
			buildAccTree(child1Idx, depth + 1, child1Triangles);
		}

	}
	const std::vector<triangle> traverse(const Ray& ray)const {
		std::stack<int> nodeIndicesToCheck;
		std::vector<triangle> triangles;
		nodeIndicesToCheck.push(0);
		while (!nodeIndicesToCheck.empty()) {
			int currentNodeIdxToCheck = nodeIndicesToCheck.top();
			nodeIndicesToCheck.pop();
			const AccTreeNode& currentNode = accTree[currentNodeIdxToCheck];
			float tHit;
			if (currentNode.aabb.checkSides(ray, tHit)) {
				if (currentNode.triangles.size() > 0) {
					//IntersectionData interData = currentNode.intersect(ray);
					triangles.insert(triangles.end(), currentNode.triangles.begin(), currentNode.triangles.end());
				}
				else {
					if (currentNode.children[0] != -1) {
						nodeIndicesToCheck.push(currentNode.children[0]);
					}
					if (currentNode.children[1] != -1) {
						nodeIndicesToCheck.push(currentNode.children[1]);
					}
				}
		}
		}
		return triangles;
	}
};