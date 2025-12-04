#ifndef SCENE_H_
#define SCENE_H_

#include "../general/general.h"

#include "triangle.h"

const unsigned int TRIANGLE_COUNT = 64;

class scene {
private:
	triangle prims[TRIANGLE_COUNT];
	glm::vec3 camera;
	glm::vec3 topRight, topLeft, bottomLeft; // from perspective of camera

public:
	scene(glm::vec3 _c) : camera(_c), 
		topRight(glm::vec3(1.0f, 1.0f, camera.z + 3.0f)),
		topLeft(glm::vec3(-1.0f, 1.0f, camera.z + 3.0f)),
		bottomLeft(glm::vec3(-1.0f, -1.0f, camera.z + 3.0f))
	{}

	void initScene() {
		for (int i = 0; i < TRIANGLE_COUNT; i++) {
			prims[i].p0 = glm::vec3(dt(rng), dt(rng), dt(rng));
			prims[i].p1 = glm::vec3(dt(rng), dt(rng), dt(rng));
			prims[i].p2 = glm::vec3(dt(rng), dt(rng), dt(rng));

			// Calculate centroid
			prims[i].calculateCentroid();
		}
	}

	void render() {
		ray r;
		for (int j = 0; j < HEIGHT; j++) { // ROWS
			for (int i = 0; i < WIDTH; i++) { // COLUMNS
				glm::vec3 pixelWorldPos =
					topLeft + (bottomLeft - topLeft) * ((float)j / HEIGHT) +
					topLeft + (topRight - topLeft) * ((float)i / WIDTH);

				r.O = camera;
				r.D = glm::normalize(pixelWorldPos - r.O);
				r.t = 1e30f;

				std::cout << "Tracing ray number " << j * WIDTH + i << " has startred, out of: " << HEIGHT * WIDTH << std::endl;
				for (int n = 0; n < TRIANGLE_COUNT; n++)
					prims[n].rayIntersection(r);

				std::cout << "Assigning color to pixel" << std::endl;
				unsigned int offset = j * WIDTH + i;
				if (r.t < 1e30f) {
					screenTexture[offset][0] = 255;
					screenTexture[offset][1] = 255;
					screenTexture[offset][2] = 255;
				}
				else
				{
					screenTexture[offset][0] = 0;
					screenTexture[offset][1] = 0;
					screenTexture[offset][2] = 0;
				}
			}
		}
	}
};

#endif // SCENE_H_