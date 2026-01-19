#ifndef BVH_H_
#define BVH_H_

#include "../general/general.h"

#include "BVHNode.h"
#include "instrumentation.h"

enum BVHType {
	NAIVE,
	SAH,
	RDH
};

class BVH {
private:
	std::vector<BVHNode> nodes;
	unsigned int nodesUsed;

	BVHType type;

public:
	BVH(BVHType _type) : type(_type), nodesUsed(1) {
		for (int i = 0; i < TRIANGLE_COUNT * 2 - 1; i++) {
			BVHNode newNode;
			nodes.push_back(newNode);
		}
	}

	void buildBVH() {
		BVHNode& root = nodes[0];
		root.leftFirst = 0;
		root.primCount = TRIANGLE_COUNT;
		root.updateBounds();

		subdivide(0);

		if (type == NAIVE)
			std::cout << "Naive BVH successfully built" << std::endl;
		if (type == SAH)
			std::cout << "SAH BVH successfully built" << std::endl;
		if (type == RDH)
			std::cout << "RDH BVH successfully built" << std::endl;
	}

	void traverse(ray& r, unsigned int nodeIdx) {
		stepCounter.increaseTraversal();

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
		// Terminate recursion NAIVE
		if (type == NAIVE && nodes[nodeIdx].primCount <= 3)
			return;

		unsigned int splitAxis;
		float splitPosition;

		BVHNode& curr = nodes[nodeIdx];

		// Splitting
		if (type == NAIVE)
			splitNaive(nodeIdx, splitAxis, splitPosition);
		if (type == SAH) {
			if (nodes[nodeIdx].primCount <= 1)
				return;

			float splitCost = splitSAH(nodeIdx, splitAxis, splitPosition);
			float parentCost = curr.primCount * curr.aabb.area();

			// Terminate recursion SAH
			if (splitCost >= parentCost)
				return;
		}
		if (type == RDH) {

		}

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

	void splitNaive(unsigned idx, unsigned int& splitAxis, float& splitPosition) {
		BVHNode& curr = nodes[idx];
		glm::vec3 diff = curr.aabb.bMax - curr.aabb.bMin;

		if (diff.x >= diff.y && diff.x >= diff.z) {
			splitAxis = 0;
			splitPosition = curr.aabb.bMin.x + diff.x * 0.5f;
			return;
		}

		if (diff.y > diff.z) {
			splitAxis = 1;
			splitPosition = curr.aabb.bMin.y + diff.y * 0.5f;
		}
		else {
			splitAxis = 2;
			splitPosition = curr.aabb.bMin.z + diff.z * 0.5f;
		}
	}

	float splitSAH(unsigned idx, unsigned int& splitAxis, float& splitPosition) {
		BVHNode& curr = nodes[idx];
		int bestAxis = -1;
		float bestPos = 0;
		float bestCost = 1e30f;

		for (unsigned int axis = 0; axis < 3; axis++) {
			for (unsigned int i = 0; i < curr.primCount; i++) {
				triangle& tri = prims[curr.leftFirst + i];
				float candidatePos = tri.centroid[axis];
				float cost = evaulateSAH(curr, axis, candidatePos);
				if (cost < bestCost) {
					bestPos = candidatePos;
					bestAxis = axis;
					bestCost = cost;
				}
			}
		}

		splitAxis = bestAxis;
		splitPosition = bestPos;
		return bestCost;
	}

	float evaulateSAH(BVHNode& curr, int axis, float pos) {
		AABB firstBox, secondBox;
		int firstCount = 0;
		int secondCount = 0;

		for (unsigned int i = 0; i < curr.primCount; i++) {
			triangle& tri = prims[curr.leftFirst + i];

			if (tri.centroid[axis] < pos) {
				firstCount++;
				firstBox.grow(tri.p0);
				firstBox.grow(tri.p1);
				firstBox.grow(tri.p2);
			}
			else
			{
				secondCount++;
				secondBox.grow(tri.p0);
				secondBox.grow(tri.p1);
				secondBox.grow(tri.p2);
			}
		}

		float cost = firstCount * firstBox.area() + secondCount * secondBox.area();
		if (cost > 0)
			return cost;

		return 1e30f;
	}

	float splitRDH(unsigned idx, unsigned int& splitAxis, float& splitPosition) {
		BVHNode& curr = nodes[idx];
		int bestAxis = -1;
		float bestPos = 0;
		float bestCost = 1e30f;

		for (unsigned int axis = 0; axis < 3; axis++) {
			for (unsigned int i = 0; i < curr.primCount; i++) {
				triangle& tri = prims[curr.leftFirst + i];
				float candidatePos = tri.centroid[axis];
				float cost = evaulateRDH(curr, axis, candidatePos);
				if (cost < bestCost) {
					bestPos = candidatePos;
					bestAxis = axis;
					bestCost = cost;
				}
			}
		}

		splitAxis = bestAxis;
		splitPosition = bestPos;
		return bestCost;
	}

	float evaulateRDH(BVHNode& curr, int axis, float pos) {
		AABB firstBox, secondBox;
		int firstCount = 0;
		int secondCount = 0;

		for (unsigned int i = 0; i < curr.primCount; i++) {
			triangle& tri = prims[curr.leftFirst + i];

			if (tri.centroid[axis] < pos) {
				firstCount++;
				firstBox.grow(tri.p0);
				firstBox.grow(tri.p1);
				firstBox.grow(tri.p2);
			}
			else
			{
				secondCount++;
				secondBox.grow(tri.p0);
				secondBox.grow(tri.p1);
				secondBox.grow(tri.p2);
			}
		}

		// Cast Ray Distribution rays to evaulate first and second box
		
		return 1e30f;
	}
};

#endif // BVH_H_