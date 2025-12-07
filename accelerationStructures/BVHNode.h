#ifndef BVH_NODE_H_

#define BVH_NODE_H_

#include "../general/general.h"

#include "../rayTracer/triangle.h"
#include "../rayTracer/ray.h"

class BVHNode {
private:
	glm::vec3 AABBmin, AABBmax;
	unsigned int leftFirst, primCount;

public:
	BVHNode() :
		AABBmin(glm::vec3(1e30f, 1e30f, 1e30f)),
		AABBmax(glm::vec3(-1e30f, -1e30f, -1e30f)),
		leftFirst(0), primCount(0)
	{
	}

	void updateBounds(triangle prims[]) {
		if (primCount == 0)
			return;

		for (int i = leftFirst; i < primCount; i++) {
			glm::vec3 triMin = prims[i].min();
			
			AABBmin.x = fminf(AABBmin.x, triMin.x);
			AABBmin.y = fminf(AABBmin.y, triMin.y);
			AABBmin.z = fminf(AABBmin.z, triMin.z);

			glm::vec3 triMax = prims[i].max();
			AABBmax.x = fmaxf(AABBmax.x, triMax.x);
			AABBmax.y = fmaxf(AABBmax.y, triMax.y);
			AABBmax.z = fmaxf(AABBmax.z, triMax.z);
		}
	}

	// Lifted from: https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
	bool intersectAABB(const ray& r) {
		float tx1 = (AABBmin.x - r.O.x) / r.D.x;
		float tx2 = (AABBmax.x - r.O.x) / r.D.x;
		float tMin = fminf(tx1, tx2);
		float tMax = fmaxf(tx1, tx2);

		float ty1 = (AABBmin.y - r.O.y) / r.D.y;
		float ty2 = (AABBmax.y - r.O.y) / r.D.y;
		tMin = fmaxf(tMin, fminf(ty1, ty2));
		tMax = fminf(tMax, fmaxf(ty1, ty2));

		float tz1 = (AABBmin.z - r.O.z) / r.D.z;
		float tz2 = (AABBmax.z - r.O.z) / r.D.z;
		tMin = fmaxf(tMin, fminf(tz1, tz2));
		tMax = fmaxf(tMax, fmaxf(tz1, tz2));

		return (tMax >= tMin) && (tMin < r.t) && (tMax > 0.0f);
	}
};

#endif // BVH_NODE_H_