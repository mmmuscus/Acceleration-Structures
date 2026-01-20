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
	RDHSAHBLEND,
	OH
};

// Class should be refactored into multiple different classes all inhereting from a base BVH class
class BVH {
private:
	unsigned int nodesUsed;
	std::vector<BVHNode> nodes;

	std::vector<unsigned int> primIdx;

	BVHType type;

	std::vector<ray> rayDistribution;
	std::vector<unsigned int> visibilityMeasures;

public:
	BVH() {}

	BVH(BVHType _type) : type(_type), nodesUsed(1) {
		for (int i = 0; i < TRIANGLE_COUNT * 2 - 1; i++) {
			BVHNode newNode;
			nodes.push_back(newNode);
		}

		for (unsigned int i = 0; i < TRIANGLE_COUNT; i++) {
			primIdx.push_back(i);
			visibilityMeasures.push_back(0);
		}
			
	}

	void buildBVH(camera* cam = NULL, int numberOfViewpoints = 20, int patternSize = 4) {
		BVHNode& root = nodes[0];
		root.leftFirst = 0;
		root.primCount = TRIANGLE_COUNT;
		root.updateBounds();

		// Create ray distributions for BVH involving RDH
		if (type == RDH || type == RDHSAHBLEND)
			createRayDistribution(cam, numberOfViewpoints, patternSize);

		if (type == OH)
			createVisibilityMeasures(cam, numberOfViewpoints);

		subdivide(0, 1);

		if (type == NAIVE)
			std::cout << "Naive BVH successfully built" << std::endl;
		if (type == SAH)
			std::cout << "SAH BVH successfully built" << std::endl;
		if (type == RDH)
			std::cout << "RDH BVH successfully built" << std::endl;
		if (type == RDHSAHBLEND)
			std::cout << "RDH blended with SAH BVH successfully built" << std::endl;
		if (type == OH)
			std::cout << "OH BVH successfully built" << std::endl;
	}

	// Assumes that camera will always look at the origin
	void createRayDistribution(camera* cam, int numberOfViewpoints, int patternSize) {
		float angleStep = 2.0f * M_PI / (float)numberOfViewpoints; // 360.0f / numberOfViewpoints * M_PI / 180.0f;

		for (int step = 0; step < numberOfViewpoints; step++) {
			cam->spin(step * angleStep);

			std::cout << "Calculating rays for viewpoint number: " << step << std::endl;

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
	}

	void createVisibilityMeasures(camera* cam, int numberOfViewpoints) {
		float angleStep = 2.0f * M_PI / (float)numberOfViewpoints; // 360.0f / numberOfViewpoints * M_PI / 180.0f;
		ray r;

		for (int step = 0; step < numberOfViewpoints; step++) {
			cam->spin(step * angleStep);
			r.O = cam->getCam();

			std::cout << "Calculating occlusion metrics for viewpoint number: " << step << std::endl;

			for (int j = 0; j < HEIGHT; j++) { // ROWS
				for (int i = 0; i < WIDTH; i++) { // COLUMNS
					glm::vec3 pixelWorldPos = cam->getPixelWorldPos(j, i);

					r.D = glm::normalize(pixelWorldPos - r.O);
					r.t = 1e30f;

					unsigned int lastPrimIdx = TRIANGLE_COUNT;

					std::cout << "Casting ray: ( " << i << " , " << j << " )\n";

					for (int n = 0; n < TRIANGLE_COUNT; n++) {
						float lastIntersection = r.t;
						prims[n].rayIntersection(r);

						if (r.t < lastIntersection) {
							std::cout << "New triangle found!" << std::endl;
							if (lastPrimIdx != TRIANGLE_COUNT)
								visibilityMeasures[lastPrimIdx]--;

							if (visibilityMeasures[n] <= step)
								visibilityMeasures[n]++;

							lastPrimIdx = n;
						}
					}
				}
			}
		}

		cam->spin(0.0f);
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

	void serialize(const std::string& filename) {
		std::ofstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			std::cout << "FAILED TO OPEN FILE" << std::endl;
			return;
		}

		file.write(reinterpret_cast<const char*>(&type), sizeof(type));
		file.write(reinterpret_cast<const char*>(&nodesUsed), sizeof(nodesUsed));

		for (unsigned int i = 0; i < nodesUsed; i++) {
			file.write(reinterpret_cast<const char*>(&nodes[i]), sizeof(nodes[i]));
		}

		file.close();
	}

	void deserialize(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			std::cout << "FAILED TO OPEN FILE" << std::endl;
			return;
		}

		file.read(reinterpret_cast<char*>(&type), sizeof(type));
		file.read(reinterpret_cast<char*>(&nodesUsed), sizeof(nodesUsed));

		for (unsigned int i = 0; i < nodesUsed; i++) {
			BVHNode newNode;
			file.read(reinterpret_cast<char*>(&newNode), sizeof(newNode));

			nodes[i] = newNode;
		}

		file.close();
	}

private:
	void subdivide(unsigned int nodeIdx, unsigned int depth) {
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

			splitCost = splitWithCost(nodeIdx, depth, splitAxis, splitPosition);
			parentCost = evaulateParentCost(curr, depth);

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
				// Ugly swap primitive
				triangle temp = prims[end - 1];
				prims[end - 1] = prims[start];
				prims[start] = temp;

				if (type == OH) {
					// Ugly swap visibility measures
					unsigned int temp = visibilityMeasures[end - 1];
					visibilityMeasures[end - 1] = visibilityMeasures[start];
					visibilityMeasures[start] = temp;
				}

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
		subdivide(leftIdx, depth + 1);
		subdivide(rightIdx, depth + 1);
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

	// Depth only needs to be passed in case of OH, abstraction would help here
	float splitWithCost(unsigned idx, unsigned int depth, unsigned int& splitAxis, float& splitPosition) {
		BVHNode& curr = nodes[idx];
		int bestAxis = -1;
		float bestPos = 0;
		float bestCost = 1e30f;

		for (unsigned int axis = 0; axis < 3; axis++) {
			for (unsigned int i = 0; i < curr.primCount; i++) {
				triangle& tri = prims[curr.leftFirst + i];
				float candidatePos = tri.centroid[axis];
				std::cout << "Evaulating candidate: " << i << " / " << curr.primCount << " for axis: " << axis << std::endl;
				float cost = evaulateCost(curr, depth, axis, candidatePos);
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

	float evaulateCost(BVHNode& curr, unsigned int depth, int axis, float pos) {
		AABB firstBox, secondBox;
		int firstCount = 0;
		int secondCount = 0;

		// For Occlusion Heuristics
		int firstVisible = 0;
		int secondVisible = 0;

		for (unsigned int i = 0; i < curr.primCount; i++) {
			triangle& tri = prims[curr.leftFirst + i];

			if (tri.centroid[axis] < pos) {
				firstCount++;
				firstBox.grow(tri.p0);
				firstBox.grow(tri.p1);
				firstBox.grow(tri.p2);

				// For Occlusion Heuristics
				firstVisible += visibilityMeasures[curr.leftFirst + i];
			}
			else
			{
				secondCount++;
				secondBox.grow(tri.p0);
				secondBox.grow(tri.p1);
				secondBox.grow(tri.p2);

				// For Occlusion Heuristics
				secondVisible += visibilityMeasures[curr.leftFirst + i];
			}
		}

		float cost = 1e30;

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

		if (type == RDHSAHBLEND) {
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
			float SAHCost = firstCount * firstBox.area() + 
				secondCount * secondBox.area();

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

		if (type == OH) {
			if (depth < (logf((float)TRIANGLE_COUNT) / 2)) {
				float firstSAHRatio = (firstCount * firstBox.area()) /
					(curr.primCount * curr.aabb.area());
				float secondSAHRatio = (secondCount * secondBox.area()) /
					(curr.primCount * curr.aabb.area());

				float w = 0.9f;
				float allVisible = (float)firstVisible + (float)secondVisible;

				float firstOcclusionCost = firstCount *
					((w * ((float)firstVisible / allVisible)) + ((1.0f - w) * firstSAHRatio));
				float secondOcclusionCost = secondCount *
					((w * ((float)secondVisible / allVisible)) + ((1.0f - w) * secondSAHRatio));

				cost = firstOcclusionCost + secondOcclusionCost;
			}
			else {
				cost = firstCount * firstBox.area() +
					secondCount * secondBox.area();
			}
		}

		if (cost > 0)
			return cost;

		return 1e30f;
	}

	float evaulateParentCost(BVHNode& curr, unsigned int depth) {
		if (type == SAH)
			return curr.primCount * curr.aabb.area();

		if (type == RDH) {
			float cost = 0;

			for (int i = 0; i < rayDistribution.size(); i++)
				if (curr.intersectAABB(rayDistribution[i]))
					cost += curr.primCount;

			return cost;
		}

		if (type == RDHSAHBLEND) {
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

		if (type == OH) { // Should not cause termination
			if (depth < (logf((float)TRIANGLE_COUNT) / 2))
				return (float)curr.primCount;
			else
				return curr.primCount * curr.aabb.area();
		}

		return 0;
	}
};

#endif // BVH_H_