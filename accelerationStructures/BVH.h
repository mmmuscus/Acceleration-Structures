#ifndef BVH_H_
#define BVH_H_

#include "../general/general.h"

#include "BVHNode.h"

class BVH {
private:
	BVHNode nodes[TRIANGLE_COUNT * 2 - 1];
	unsigned int nodesUsed;

public:
	BVH() : nodesUsed(1) {}

	void buildBVH() {
		BVHNode& root = nodes[0];
		root.leftFirst = 0; 
		root.primCount = TRIANGLE_COUNT;
		root.updateBounds();

		subdivide(0);

		std::cout << "END" << std::endl;
	}

	void traverse(ray& r, unsigned int nodeIdx) {
		BVHNode& curr = nodes[nodeIdx];

		if (!curr.intersectAABB(r))
			return;

		if (curr.primCount != 0) {
			for (int i = 0; i < curr.primCount; i++) {
				prims[curr.leftFirst + i].rayIntersection(r);
			}
		}
		else {
			traverse(r, curr.leftFirst);
			traverse(r, curr.leftFirst + 1);
		}
	}

private:
	void subdivide(unsigned int nodeIdx) {
		if (nodes[nodeIdx].primCount <= 3)
			return;

		unsigned int splitAxis;
		float splitPosition;

		split(nodeIdx, splitAxis, splitPosition);

		BVHNode& curr = nodes[nodeIdx];

		// Swap triangles around
		unsigned int start = curr.leftFirst;
		unsigned int end = start + curr.primCount;
		while (start < end) {
			if (prims[start].centroid[splitAxis] < splitPosition) {
				start++;
			}
			else {
				// Ugly swap
				triangle temp = prims[end - 1];
				prims[end - 1] = prims[start];
				prims[start] = temp;
				end--;
			}
		}

		// We dont store isleaf, need to not have leaves with 0 primitives
		unsigned int leftCount = start - curr.leftFirst;
		if (leftCount == 0 || leftCount == curr.primCount)
			return;

		// "Create" new child nodes, assign values correctly
		unsigned int leftIdx = nodesUsed++;
		unsigned int rightIdx = nodesUsed++;
		BVHNode& left = nodes[leftIdx];
		left.leftFirst = curr.leftFirst;
		left.primCount = leftCount;
		left.updateBounds();
		curr.leftFirst = leftIdx;
		BVHNode& right = nodes[rightIdx];
		right.leftFirst = left.leftFirst + left.primCount;
		right.primCount = curr.primCount - left.primCount;
		right.updateBounds();
		curr.primCount = 0;

		// Recurse
		subdivide(leftIdx);
		subdivide(rightIdx);
	}

	void split(unsigned idx, unsigned int& splitAxis, float& splitPosition) {
		BVHNode& curr = nodes[idx];
		glm::vec3 diff = curr.AABBmax - curr.AABBmin;

		if (diff.x >= diff.y && diff.x >= diff.z) {
			splitAxis = 0;
			splitPosition = curr.AABBmin.x + diff.x * 0.5f;
			return;
		}

		if (diff.y > diff.z) {
			splitAxis = 1;
			splitPosition = curr.AABBmin.y + diff.y * 0.5f;
		}
		else {
			splitAxis = 2;
			splitPosition = curr.AABBmin.z + diff.z * 0.5f;
		}
	}
};

#endif // BVH_H_