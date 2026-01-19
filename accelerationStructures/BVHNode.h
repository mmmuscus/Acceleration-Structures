#ifndef BVH_NODE_H
#define BVH_NODE_H_

#include "../general/general.h"

#include "../rayTracer/triangle.h"
#include "../rayTracer/ray.h"

struct AABB {
	glm::vec3 bMin = glm::vec3(1e30f, 1e30f, 1e30f);
	glm::vec3 bMax = glm::vec3(-1e30f, -1e30f, -1e30f);

	void grow(glm::vec3 p) {
		bMin.x = fminf(bMin.x, p.x);
		bMin.y = fminf(bMin.y, p.y);
		bMin.z = fminf(bMin.z, p.z);

		bMax.x = fmaxf(bMax.x, p.x);
		bMax.y = fmaxf(bMax.y, p.y);
		bMax.z = fmaxf(bMax.z, p.z);
	}

	float area() {
		glm::vec3 extent = bMax - bMin;
		return extent.x * extent.y + extent.y * extent.z + extent.z * extent.x;
	}
};

class BVHNode {
public:
	AABB aabb;
	//glm::vec3 AABBmin, AABBmax;
	unsigned int leftFirst, primCount;

	BVHNode() : leftFirst(0), primCount(0) {
		aabb.bMin = glm::vec3(1e30f, 1e30f, 1e30f);
		aabb.bMax = glm::vec3(-1e30f, -1e30f, -1e30f);
	}

	void updateBounds() {
		if (primCount == 0)
			return;

		int end = leftFirst + primCount;
		for (int i = leftFirst; i < end; i++) {
			glm::vec3 triMin = prims[i].min();
			
			aabb.bMin.x = fminf(aabb.bMin.x, triMin.x);
			aabb.bMin.y = fminf(aabb.bMin.y, triMin.y);
			aabb.bMin.z = fminf(aabb.bMin.z, triMin.z);

			glm::vec3 triMax = prims[i].max();
			aabb.bMax.x = fmaxf(aabb.bMax.x, triMax.x);
			aabb.bMax.y = fmaxf(aabb.bMax.y, triMax.y);
			aabb.bMax.z = fmaxf(aabb.bMax.z, triMax.z);
		}
	}

	// Lifted from: https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
	bool intersectAABB(const ray& r) {
		float tx1 = (aabb.bMin.x - r.O.x) / r.D.x;
		float tx2 = (aabb.bMax.x - r.O.x) / r.D.x;
		float tMin = fminf(tx1, tx2);
		float tMax = fmaxf(tx1, tx2);

		float ty1 = (aabb.bMin.y - r.O.y) / r.D.y;
		float ty2 = (aabb.bMax.y - r.O.y) / r.D.y;
		tMin = fmaxf(tMin, fminf(ty1, ty2));
		tMax = fminf(tMax, fmaxf(ty1, ty2));

		float tz1 = (aabb.bMin.z - r.O.z) / r.D.z;
		float tz2 = (aabb.bMax.z - r.O.z) / r.D.z;
		tMin = fmaxf(tMin, fminf(tz1, tz2));
		tMax = fminf(tMax, fmaxf(tz1, tz2));

		return (tMax >= tMin) && (tMin < r.t) && (tMax > 0.0f);
	}
};

#endif // BVH_NODE_H_