#ifndef BVH_H_
#define BVH_H_

#include "../general/general.h"

#include "../rayTracer/camera.h"

#include "BVHNode.h"
#include "instrumentation.h"

enum BVHType {
	NAIVE,
	SAH,
	RDH,
	RDHSAHBlend
};

class BVH {
private:
	std::vector<BVHNode> nodes;
	unsigned int nodesUsed;

	BVHType type;
	std::vector<ray> rayDistribution;

public:
	BVH(BVHType _type) : type(_type), nodesUsed(1) {
		for (int i = 0; i < TRIANGLE_COUNT * 2 - 1; i++) {
			BVHNode newNode;
			nodes.push_back(newNode);
		}
	}

	void buildBVH(camera* cam = NULL, int numberOfViewpoints = 20, int patternSize = 4) {
		BVHNode& root = nodes[0];
		root.leftFirst = 0;
		root.primCount = TRIANGLE_COUNT;
		root.updateBounds();

		// Create ray distributions for BVH involving RDH
		if (type == RDH || type == RDHSAHBlend)
			createRayDistribution(cam, numberOfViewpoints, patternSize);

		subdivide(0);

		if (type == NAIVE)
			std::cout << "Naive BVH successfully built" << std::endl;
		if (type == SAH)
			std::cout << "SAH BVH successfully built" << std::endl;
		if (type == RDH)
			std::cout << "RDH BVH successfully built" << std::endl;
		if (type == RDHSAHBlend)
			std::cout << "RDH blended with SAH BVH successfully built" << std::endl;
	}

	// Assumes that camera will always look at the origin
	void createRayDistribution(camera* cam, int numberOfViewpoints, int patternSize) {
		float angleStep = 2.0f * M_PI / (float)numberOfViewpoints; // 360.0f / numberOfViewpoints * M_PI / 180.0f;

		std::cout << "START OF RAYDISTRIBUTION BUILDING" << std::endl;

		for (int i = 0; i < numberOfViewpoints; i++) {
			cam->spin(i * angleStep);

			std::cout << "Calculating rays for viewpoint number: " << i << std::endl;

			for (int j = 0; j < HEIGHT; j += patternSize) { // ROWS
				for (int i = 0; i < WIDTH; i += patternSize) { // COLUMNS
					glm::vec3 pixelWorldPos = cam->getPixelWorldPos(j, i);

					ray newRay;
					newRay.O = cam->getCam();
					newRay.D = glm::normalize(pixelWorldPos - newRay.O);
					newRay.t = 1e30f;

					rayDistribution.push_back(newRay);
				}
			}
		}

		// Reset camera
		cam->spin(0.0f);

		std::cout << "END OF RAYDISTRIBUTION BUILDING" << std::endl;
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
		float splitCost, parentCost;

		BVHNode& curr = nodes[nodeIdx];

		// Splitting
		std::cout << "Splitting node: " << nodeIdx << std::endl;

		if (type == NAIVE)
			splitNaive(nodeIdx, splitAxis, splitPosition);
		else {
			if (nodes[nodeIdx].primCount <= 1)
				return;

			splitCost = splitWithCost(nodeIdx, splitAxis, splitPosition);
			parentCost = evaulateParentCost(curr);

			if (splitCost > parentCost)
				return;
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

	float splitWithCost(unsigned idx, unsigned int& splitAxis, float& splitPosition) {
		BVHNode& curr = nodes[idx];
		int bestAxis = -1;
		float bestPos = 0;
		float bestCost = 1e30f;

		for (unsigned int axis = 0; axis < 3; axis++) {
			for (unsigned int i = 0; i < curr.primCount; i++) {
				triangle& tri = prims[curr.leftFirst + i];
				float candidatePos = tri.centroid[axis];
				std::cout << "Evaulating candidate: " << i << " / " << curr.primCount << " for axis: " << axis << std::endl;
				float cost = evaulateCost(curr, axis, candidatePos);
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

	float evaulateCost(BVHNode& curr, int axis, float pos) {
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

		float cost = 0;

		if (type == SAH)
			cost = firstCount * firstBox.area() + secondCount * secondBox.area();
		
		if (type == RDH) {
			int firstRayIntersectionCount = 0;
			int secondRayIntersectionCount = 0;

			for (int i = 0; i < rayDistribution.size(); i++) {
				if (firstBox.intersect(rayDistribution[i]))
					firstRayIntersectionCount++;

				if (secondBox.intersect(rayDistribution[i]))
					secondRayIntersectionCount++;
			}

			cost = firstCount * firstRayIntersectionCount +
				secondCount * secondRayIntersectionCount;
		}

		if (type == RDHSAHBlend) {
			int firstRayIntersectionCount = 0;
			int secondRayIntersectionCount = 0;

			for (int i = 0; i < rayDistribution.size(); i++) {
				if (firstBox.intersect(rayDistribution[i]))
					firstRayIntersectionCount++;

				if (secondBox.intersect(rayDistribution[i]))
					secondRayIntersectionCount++;
			}

			// Calculate SAH and RDH costs
			float RDHCost = firstCount * firstRayIntersectionCount +
				secondCount * secondRayIntersectionCount;
			float SAHCost = firstCount * firstBox.area() + secondCount * secondBox.area();

			// Calculate blend weight
			int R = 0; // Number of rays intersecting parent
			for (int i = 0; i < rayDistribution.size(); i++)
				if (curr.intersectAABB(rayDistribution[i]))
					R++;

			float alpha = 0.9f;
			float beta = 0.1f;
			float w = alpha * (1.0f - (1.0f / (1.0f + beta * (float)R)));

			// Final cost			
			cost = w * RDHCost + (1.0f - w) * SAHCost;
		}

		if (cost > 0)
			return cost;

		return 1e30f;
	}

	float evaulateParentCost(BVHNode& curr) {
		if (type == SAH)
			return curr.primCount * curr.aabb.area();

		if (type == RDH) {
			float cost = 0;

			for (int i = 0; i < rayDistribution.size(); i++)
				if (curr.intersectAABB(rayDistribution[i]))
					cost += curr.primCount;

			return cost;
		}

		if (type == RDHSAHBlend) {
			int R = 0;  // Number of rays intersecting parent

			for (int i = 0; i < rayDistribution.size(); i++)
				if (curr.intersectAABB(rayDistribution[i]))
					R++;

			// Calculate SAH and RDH costs
			float RDHCost = (float)R * curr.primCount;
			float SAHCost = curr.primCount * curr.aabb.area();

			// Calculate blend weight
			float alpha = 0.9f;
			float beta = 0.1f;
			float w = alpha * (1.0f - (1.0f / (1.0f + beta * (float)R)));

			// Final cost
			return w * RDHCost + (1.0f - w) * SAHCost;
		}

		return 0;
	}
};

#endif // BVH_H_