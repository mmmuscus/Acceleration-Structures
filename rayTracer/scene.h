#ifndef SCENE_H_
#define SCENE_H_

#include "../general/general.h"

#include "triangle.h"

const unsigned int TRIANGLE_COUNT = 64;

class scene {
private:
	triangle prims[TRIANGLE_COUNT];

public:
	void initScene() {
		for (int i = 0; i < TRIANGLE_COUNT; i++) {
			prims[i].p1 = glm::vec3(dt(rng), dt(rng), dt(rng));
			prims[i].p2 = glm::vec3(dt(rng), dt(rng), dt(rng));
			prims[i].p3 = glm::vec3(dt(rng), dt(rng), dt(rng));
		}
	}
};

#endif // SCENE_H_