#ifndef SCENE_H_
#define SCENE_H_

#include "../general/general.h"

#include "../accelerationStructures/BVH.h"

#include "triangle.h"

class scene {
private:
	glm::vec3 camera;
	glm::vec3 topRight, topLeft, bottomLeft; // from perspective of camera

public:
	scene(glm::vec3 _c) : camera(_c), 
		topRight(glm::vec3(1.0f, 1.0f, camera.z + 5.0f)),
		topLeft(glm::vec3(-1.0f, 1.0f, camera.z + 5.0f)),
		bottomLeft(glm::vec3(-1.0f, -1.0f, camera.z + 5.0f))
	{}

	// Guided by: https://vulkan-tutorial.com/Loading_models
	// Also by: https://github.com/canmom/rasteriser/blob/master/fileloader.cpp
	void initScene() {
		std::cout << "Loading model started" << std::endl;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		// Source of .obj: https://graphics.stanford.edu/courses/cs148-10-summer/as3/code/as3/teapot.obj
		std::string path = "assets/teapot.obj";

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
			throw std::runtime_error(err);
		}

		std::vector<glm::vec3> vertices;

		// Load vertices
		for (const auto& shape : shapes) {
			const std::vector<tinyobj::index_t>& indices = shape.mesh.indices;
			const std::vector<int>& mat_ids = shape.mesh.material_ids;

			for (size_t face_ind = 0; face_ind < mat_ids.size(); face_ind++) {
				triangle prim;

				prim.p0 = glm::vec3(
					attrib.vertices[3 * indices[3 * face_ind].vertex_index + 0],
					attrib.vertices[3 * indices[3 * face_ind].vertex_index + 1],
					attrib.vertices[3 * indices[3 * face_ind].vertex_index + 2]
				);
				prim.p1 = glm::vec3(
					attrib.vertices[3 * indices[3 * face_ind + 1].vertex_index + 0],
					attrib.vertices[3 * indices[3 * face_ind + 1].vertex_index + 1],
					attrib.vertices[3 * indices[3 * face_ind + 1].vertex_index + 2]
				);
				prim.p2 = glm::vec3(
					attrib.vertices[3 * indices[3 * face_ind + 2].vertex_index + 0],
					attrib.vertices[3 * indices[3 * face_ind + 2].vertex_index + 1],
					attrib.vertices[3 * indices[3 * face_ind + 2].vertex_index + 2]
				);
				// Shift model down and flip it
				prim.p0.y = 1.5f - prim.p0.y;
				prim.p1.y = 1.5f - prim.p1.y;
				prim.p2.y = 1.5f - prim.p2.y;
				prim.calculateCentroid();

				prims.push_back(prim);

				// Triangle for second teapot
				triangle prim2 = prim;
				prim2.p0 += glm::vec3(1.5f, 1.5f, 1.5f);
				prim2.p1 += glm::vec3(1.5f, 1.5f, 1.5f);
				prim2.p2 += glm::vec3(1.5f, 1.5f, 1.5f);
				prim2.calculateCentroid();

				prims.push_back(prim2);
			}
		}

		TRIANGLE_COUNT = prims.size();
	}

	void render(BVH& bvh, bool useBVH) {
		stepCounter.reset();
		ray r;

		for (int j = 0; j < HEIGHT; j++) { // ROWS
			for (int i = 0; i < WIDTH; i++) { // COLUMNS
				glm::vec3 pixelWorldPos = topLeft + 
					(bottomLeft - topLeft) * ((float)j / HEIGHT) +
					(topRight - topLeft) * ((float)i / WIDTH);

				r.O = camera;
				r.D = glm::normalize(pixelWorldPos - r.O);
				r.t = 1e30f;

				if (useBVH)
					bvh.traverse(r, 0);
				else
				{
					for (int n = 0; n < TRIANGLE_COUNT; n++)
						prims[n].rayIntersection(r);
				}

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

		stepCounter.print();
	}
};

#endif // SCENE_H_