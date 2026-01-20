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

	unsigned int primIdxSize;
	std::vector<unsigned int> primIdx;

	BVHType type;

	std::vector<ray> rayDistribution;
	std::vector<bool> intersectedParent; // If corresponding ray from distribution intersected the parent
	std::vector<unsigned int> visibilityMeasures;
	std::vector<unsigned int> potentialPrimIdxs;

	// Measurements for building BVH
	unsigned int costEvals;
	unsigned int AABBIntersectionCount;
	unsigned int primIntersectionCount;

public:
	BVH(BVHType _type) : type(_type), nodesUsed(1),
		costEvals(0), AABBIntersectionCount(0), primIntersectionCount(0)
	{
		for (int i = 0; i < TRIANGLE_COUNT * 2 - 1; i++) {
			BVHNode newNode;
			nodes.push_back(newNode);
		}

		for (unsigned int i = 0; i < TRIANGLE_COUNT; i++) {
			primIdx.push_back(i);
			visibilityMeasures.push_back(0);
		}

		primIdxSize = primIdx.size();

	}

	void buildAndSerialize(
		const std::string& filename,
		camera* cam = NULL, int numberOfViewpoints = 20, int patternSize = 4
	) {
		buildBVH(cam, numberOfViewpoints, patternSize);
		serialize(filename);
	}

	void buildBVH(camera* cam = NULL, int numberOfViewpoints = 20, int patternSize = 4) {
		BVHNode& root = nodes[0];
		root.leftFirst = 0;
		root.primCount = TRIANGLE_COUNT;
		updateBounds(root);

		if (type == NAIVE)
			std::cout << "Building naive BVH started" << std::endl;
		if (type == SAH)
			std::cout << "Building SAH BVH started" << std::endl;
		if (type == RDH)
			std::cout << "Building RDH BVH started" << std::endl;
		if (type == RDHSAHBLEND)
			std::cout << "Building RDH blended with SAH BVH started" << std::endl;
		if (type == OH)
			std::cout << "Building OH BVH started" << std::endl;

		// Create ray distributions for BVH involving RDH
		if (type == RDH || type == RDHSAHBLEND)
			createRayDistribution(cam, numberOfViewpoints, patternSize);

		if (type == OH)
			createVisibilityMeasures(cam, numberOfViewpoints);

		std::cout << "Subdivision started" << std::endl;

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

	void traverse(ray& r, unsigned int nodeIdx) {
		stepCounter.increaseTraversal();

		BVHNode& curr = nodes[nodeIdx];

		if (!curr.intersectAABB(r))
			return;

		if (curr.primCount != 0) {
			for (int i = 0; i < curr.primCount; i++) {
				prims[primIdx[curr.leftFirst + i]].rayIntersection(r);
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

		file.write(reinterpret_cast<const char*>(&primIdxSize), sizeof(primIdxSize));

		for (unsigned int i = 0; i < primIdxSize; i++) {
			file.write(reinterpret_cast<const char*>(&primIdx[i]), sizeof(primIdx[i]));
		}

		file.write(reinterpret_cast<const char*>(&costEvals), sizeof(costEvals));
		file.write(reinterpret_cast<const char*>(&AABBIntersectionCount), sizeof(AABBIntersectionCount));
		file.write(reinterpret_cast<const char*>(&primIntersectionCount), sizeof(primIntersectionCount));

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
			file.read(reinterpret_cast<char*>(&nodes[i]), sizeof(nodes[i]));
		}

		file.read(reinterpret_cast<char*>(&primIdxSize), sizeof(primIdxSize));

		for (unsigned int i = 0; i < primIdxSize; i++) {
			file.read(reinterpret_cast<char*>(&primIdx[i]), sizeof(primIdx[i]));
		}

		file.read(reinterpret_cast<char*>(&costEvals), sizeof(costEvals));
		file.read(reinterpret_cast<char*>(&AABBIntersectionCount), sizeof(AABBIntersectionCount));
		file.read(reinterpret_cast<char*>(&primIntersectionCount), sizeof(primIntersectionCount));

		file.close();
	}

private:
	// Assumes that camera will always look at the origin
	void createRayDistribution(camera* cam, int numberOfViewpoints, int patternSize) {
		float angleStep = 2.0f * M_PI / (float)numberOfViewpoints; // 360.0f / numberOfViewpoints * M_PI / 180.0f;

		for (int step = 0; step < numberOfViewpoints; step++) {
			cam->spin(step * angleStep);

			for (int j = 0; j < HEIGHT; j += patternSize) { // ROWS
				for (int i = 0; i < WIDTH; i += patternSize) { // COLUMNS
					glm::vec3 pixelWorldPos = cam->getPixelWorldPos(j, i);

					ray newRay;
					newRay.O = cam->getCam();
					newRay.D = glm::normalize(pixelWorldPos - newRay.O);
					newRay.t = 1e30f;

					rayDistribution.push_back(newRay);
					intersectedParent.push_back(false);
				}
			}
		}

		// Reset camera
		cam->spin(0.0f);
	}

	void createVisibilityMeasures(camera* cam, int numberOfViewpoints) {
		// build temporary bvh for speeding up creation of visibility measures:
		BVH tempBvh = BVH(SAH);
		tempBvh.buildBVH();

		costEvals += tempBvh.costEvals;

		float angleStep = 2.0f * M_PI / (float)numberOfViewpoints; // 360.0f / numberOfViewpoints * M_PI / 180.0f;
		ray r;

		for (int step = 0; step < numberOfViewpoints; step++) {
			cam->spin(step * angleStep);
			r.O = cam->getCam();

			std::vector<unsigned int> tempVisMeas;
			for (int i = 0; i < TRIANGLE_COUNT; i++)
				tempVisMeas.push_back(0);

			for (int j = 0; j < HEIGHT; j++) { // ROWS
				for (int i = 0; i < WIDTH; i++) { // COLUMNS
					glm::vec3 pixelWorldPos = cam->getPixelWorldPos(j, i);

					r.D = glm::normalize(pixelWorldPos - r.O);
					r.t = 1e30f;

					tempBvh.potentialPrimIdxs.clear();
					tempBvh.traverseForOHConstruction(r, 0);

					unsigned int lastPrimIdx = TRIANGLE_COUNT;

					for (int n = 0; n < tempBvh.potentialPrimIdxs.size(); n++) {
						float lastIntersection = r.t;
						prims[tempBvh.potentialPrimIdxs[n]].rayIntersection(r);
						primIntersectionCount++;

						if (r.t < lastIntersection) {
							if (lastPrimIdx != TRIANGLE_COUNT)
								tempVisMeas[lastPrimIdx]--;

							tempVisMeas[tempBvh.potentialPrimIdxs[n]]++;
							lastPrimIdx = tempBvh.potentialPrimIdxs[n];
						}
					}
				}
			}

			for (int n = 0; n < TRIANGLE_COUNT; n++) {
				if (tempVisMeas[n] > 0)
					visibilityMeasures[n]++;
			}
		}

		AABBIntersectionCount += tempBvh.AABBIntersectionCount;
		cam->spin(0.0f);
	}

	void traverseForOHConstruction(ray& r, unsigned int nodeIdx) {
		AABBIntersectionCount++;

		BVHNode& curr = nodes[nodeIdx];

		if (!curr.intersectAABB(r))
			return;

		if (curr.primCount != 0) {
			for (int i = 0; i < curr.primCount; i++) {
				potentialPrimIdxs.push_back(primIdx[curr.leftFirst + i]);
			}
		}
		else {
			traverseForOHConstruction(r, curr.leftFirst);
			traverseForOHConstruction(r, curr.leftFirst + 1);
		}
	}

	void subdivide(unsigned int nodeIdx, unsigned int depth) {
		// Terminate recursion NAIVE
		if (type == NAIVE && nodes[nodeIdx].primCount <= 3)
			return;

		unsigned int splitAxis;
		float splitPosition;
		float splitCost, parentCost;

		BVHNode& curr = nodes[nodeIdx];

		// Splitting
		if (type == NAIVE)
			splitNaive(nodeIdx, splitAxis, splitPosition);
		else {
			if (nodes[nodeIdx].primCount <= 1)
				return;

			int R = 0;
			if (type == RDHSAHBLEND || type == RDH) {
				resetIntersectedParent();
				R = calculateR(curr);
			}

			parentCost = evaulateParentCost(curr, depth, R);
			splitCost = splitWithCost(nodeIdx, depth, splitAxis, splitPosition, R);

			if (splitCost > parentCost)
				return;
		}

		// Swap triangles around
		unsigned int start = curr.leftFirst;
		unsigned int end = start + curr.primCount;
		while (start < end) {
			if (prims[primIdx[start]].centroid[splitAxis] < splitPosition) {
				start++;
			}
			else {
				std::swap(primIdx[end - 1], primIdx[start]);

				if (type == OH)
					std::swap(visibilityMeasures[end - 1], visibilityMeasures[start]);

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
		updateBounds(left);
		curr.leftFirst = leftIdx;
		BVHNode& right = nodes[rightIdx];
		right.leftFirst = left.leftFirst + left.primCount;
		right.primCount = curr.primCount - left.primCount;
		updateBounds(right);
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
	float splitWithCost(
		unsigned idx, unsigned int depth, 
		unsigned int& splitAxis, float& splitPosition,
		int R
	) {
		BVHNode& curr = nodes[idx];
		int bestAxis = -1;
		float bestPos = 0;
		float bestCost = 1e30f;

		for (unsigned int axis = 0; axis < 3; axis++) {
			for (unsigned int i = 0; i < curr.primCount; i++) {
				triangle& tri = prims[primIdx[curr.leftFirst + i]];
				float candidatePos = tri.centroid[axis];
				float cost = evaulateCost(curr, depth, axis, candidatePos, R);
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

	float evaulateCost(BVHNode& curr, unsigned int depth, int axis, float pos, int R) {
		costEvals++;

		AABB firstBox, secondBox;
		int firstCount = 0;
		int secondCount = 0;

		// For Occlusion Heuristics
		int firstVisible = 0;
		int secondVisible = 0;

		for (unsigned int i = 0; i < curr.primCount; i++) {
			triangle& tri = prims[primIdx[curr.leftFirst + i]];

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
				if (intersectedParent[i]) {
					if (firstBox.intersect(rayDistribution[i]))
						firstRayIntersectionCount++;

					if (secondBox.intersect(rayDistribution[i]))
						secondRayIntersectionCount++;

					AABBIntersectionCount += 2;
				}
			}

			cost = firstCount * firstRayIntersectionCount +
				secondCount * secondRayIntersectionCount;
		}

		if (type == RDHSAHBLEND) {
			int firstRayIntersectionCount = 0;
			int secondRayIntersectionCount = 0;

			for (int i = 0; i < rayDistribution.size(); i++) {
				if (intersectedParent[i]) {
					if (firstBox.intersect(rayDistribution[i]))
						firstRayIntersectionCount++;

					if (secondBox.intersect(rayDistribution[i]))
						secondRayIntersectionCount++;

					AABBIntersectionCount += 2;
				}
			}

			// Calculate SAH and RDH costs
			float RDHCost = firstCount * firstRayIntersectionCount +
				secondCount * secondRayIntersectionCount;
			float SAHCost = firstCount * firstBox.area() + 
				secondCount * secondBox.area();

			// Calculate blend weight
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

	float evaulateParentCost(BVHNode& curr, unsigned int depth, int R) {
		costEvals++;

		if (type == SAH)
			return curr.primCount * curr.aabb.area();

		if (type == RDH) 
			return R * curr.primCount;

		if (type == RDHSAHBLEND) {
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

	int calculateR(BVHNode& curr) {
		int R = 0;

		for (int i = 0; i < rayDistribution.size(); i++)
			if (curr.intersectAABB(rayDistribution[i])) {
				R++;
				intersectedParent[i] = true;
			}

		AABBIntersectionCount += rayDistribution.size();

		return R;
	}

	void resetIntersectedParent() {
		for (int i = 0; i < intersectedParent.size(); i++)
			intersectedParent[i] = false;
	}

	void updateBounds(BVHNode& curr) {
		if (curr.primCount == 0)
			return;

		glm::vec3 min = glm::vec3(1e30f, 1e30f, 1e30f);
		glm::vec3 max = glm::vec3(-1e30f, -1e30f, -1e30f);

		int end = curr.leftFirst + curr.primCount;
		for (int i = curr.leftFirst; i < end; i++) {
			glm::vec3 primMin = prims[primIdx[i]].min();
			min.x = fminf(min.x, primMin.x);
			min.y = fminf(min.y, primMin.y);
			min.z = fminf(min.z, primMin.z);

			glm::vec3 primMax = prims[primIdx[i]].max();
			max.x = fmaxf(max.x, primMax.x);
			max.y = fmaxf(max.y, primMax.y);
			max.z = fmaxf(max.z, primMax.z);
		}

		curr.updateBounds(min, max);
	}
};

#endif // BVH_H_