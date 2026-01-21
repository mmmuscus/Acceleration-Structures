#ifndef SCENE_H_
#define SCENE_H_

#include "../general/general.h"

#include "../accelerationStructures/BVH.h"

#include "triangle.h"
#include "camera.h"

class scene {
private:
	camera* cam;

public:
	scene(glm::vec3 _c) {
		cam = new camera(_c);
	}

	camera* getCam() { return cam; }

	void setSceneGeometry(std::vector<triangle>& sceneToLoad) {
		prims.clear();
		prims = sceneToLoad;
		TRIANGLE_COUNT = prims.size();
	}

	void render(BVH& bvh) {
		stepCounter.reset();
		ray r;

		for (int j = 0; j < HEIGHT; j++) { // ROWS
			for (int i = 0; i < WIDTH; i++) { // COLUMNS
				stepCounter.setHeightWidth(j, i);

				glm::vec3 pixelWorldPos = cam->getPixelWorldPos(j, i);

				r.O = cam->getCam();
				r.D = glm::normalize(pixelWorldPos - r.O);
				r.t = 1e30f;

				bvh.traverse(r, 0);

				unsigned int offset = j * WIDTH + i;
				if (r.t < 1e30f) {
					glm::vec3 hitPoint = r.O + (r.t * r.D);
					float rComp = (hitPoint.x + 5.0f) * 255.0f / 10.0f;
					float gComp = (hitPoint.y + 5.0f) * 255.0f / 10.0f;

					textures[0][offset][0] = rComp;
					textures[0][offset][1] = gComp;
					textures[0][offset][2] = 128;
				}
				else
				{
					textures[0][offset][0] = 0;
					textures[0][offset][1] = 0;
					textures[0][offset][2] = 0;
				}
			}
		}

		stepCounter.calculateValues();
		//stepCounter.print();
		stepCounter.createTextures();
	}
};

#endif // SCENE_H_