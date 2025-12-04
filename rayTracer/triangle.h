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
};

#endif // TRIANGLE_H_ 