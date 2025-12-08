#ifndef SCENE_H_
#define SCENE_H_

#include "../general/general.h"

#include "../accelerationStructures/BVH.h"

#include "triangle.h"

class scene {
private:
	glm::vec3 ogCamera;
	glm::vec3 ogTopRight, ogTopLeft, ogBottomLeft;
	glm::vec3 pivot;

	glm::vec3 camera;
	glm::vec3 topRight, topLeft, bottomLeft; // from perspective of camera

public:
	scene(glm::vec3 _c) : ogCamera(_c), pivot(glm::vec3(0.0f, 0.0f, 0.0f)),
		ogTopRight(glm::vec3(ogCamera.x + 1.0f, ogCamera.x + 1.0f, ogCamera.z + 4.0f)),
		ogTopLeft(glm::vec3(ogCamera.x - 1.0f, ogCamera.x + 1.0f, ogCamera.z + 4.0f)),
		ogBottomLeft(glm::vec3(ogCamera.x - 1.0f, ogCamera.x - 1.0f, ogCamera.z + 4.0f)),
		camera(ogCamera), 
		topRight(ogTopRight), topLeft(ogTopLeft), bottomLeft(ogBottomLeft)
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
				prim.p0.y = 1.5f - prim.p0.y + 2.0f;
				prim.p1.y = 1.5f - prim.p1.y + 2.0f;
				prim.p2.y = 1.5f - prim.p2.y + 2.0f;
				prim.calculateCentroid();

				prims.push_back(prim);

				// Triangle for second teapot
				triangle prim2 = prim;
				prim2.p0 += glm::vec3(0.0f, -4.0f, 0.0f);
				prim2.p1 += glm::vec3(0.0f, -4.0f, 0.0f);
				prim2.p2 += glm::vec3(0.0f, -4.0f, 0.0f);
				prim2.calculateCentroid();

				prims.push_back(prim2);
			}
		}

		TRIANGLE_COUNT = prims.size();
	}

	// Guided by https://stackoverflow.com/questions/50473848/rotate-point-around-pivot-point-repeatedly
	// Only rotates coordinates x and y, z should not change for our use case
	void spinCamera(float rad) {
		camera = spinPoint(rad, ogCamera);
		topRight = spinPoint(rad, ogTopRight);
		topLeft = spinPoint(rad, ogTopLeft);
		bottomLeft = spinPoint(rad, ogBottomLeft);
	}

	glm::vec3 spinPoint(float rad, glm::vec3 point) {
		float cosTheta = cos(rad);
		float sinTheta = sin(rad);

		float x = (cosTheta * (point.x - pivot.x) - sinTheta * (point.y - pivot.y) + pivot.x);
		float y = (sinTheta * (point.x - pivot.x) + cosTheta * (point.y - pivot.y) + pivot.y);
	
		return glm::vec3(x, y, point.z);
	}

	void render(BVH& bvh, bool useBVH) {
		stepCounter.reset();
		ray r;

		for (int j = 0; j < HEIGHT; j++) { // ROWS
			for (int i = 0; i < WIDTH; i++) { // COLUMNS
				stepCounter.setHeightWidth(j, i);

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
					textures[0][offset][0] = 255;
					textures[0][offset][1] = 255;
					textures[0][offset][2] = 255;
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
		stepCounter.print();
		stepCounter.createTextures();
	}
};

#endif // SCENE_H_