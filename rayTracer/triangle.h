#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../general/general.h"

#include "ray.h"

class triangle {
public:
	glm::vec3 p0, p1, p2;
	glm::vec3 centroid;

	void calculateCentroid() {
		centroid = (p0 + p1 + p2) * 0.333333f;
	}

	// Lifted from https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
	void rayIntersection(ray& r) {
		const glm::vec3 edge1 = p1 - p0;
		const glm::vec3 edge2 = p2 - p0;
		const glm::vec3 h = glm::cross(r.D, edge2);
		const float a = glm::dot(edge1, h);
		if (a > -EPSILON && a < EPSILON)
			return; // ray parallel to triangle

		const float f = 1.0f / a;
		const glm::vec3 s = r.O - p0;
		const float u = f * glm::dot(s, h);
		if (u < 0 || u > 1)
			return;

		const glm::vec3 q = glm::cross(s, edge1);
		const float v = f * glm::dot(r.D, q);
		if (v < 0 || u + v > 1)
			return;

		const float t = f * glm::dot(edge2, q);
		if (t > EPSILON)
			r.t = glm::min(r.t, t);
	}

	glm::vec3 min() {
		glm::vec3 min;

		min.x = fminf(p0.x, p1.x);
		min.x = fminf(min.x, p2.x);

		min.y = fminf(p0.y, p1.y);
		min.y = fminf(min.y, p2.y);

		min.z = fminf(p0.z, p1.z);
		min.z = fminf(min.z, p2.z);

		return min;
	}

	glm::vec3 max() {
		glm::vec3 max;

		max.x = fmaxf(p0.x, p1.x);
		max.x = fmaxf(max.x, p2.x);

		max.y = fmaxf(p0.y, p1.y);
		max.y = fmaxf(max.y, p2.y);

		max.z = fmaxf(p0.z, p1.z);
		max.z = fmaxf(max.z, p2.z);

		return max;
	}
};

triangle prims[TRIANGLE_COUNT];

#endif // TRIANGLE_H_ 