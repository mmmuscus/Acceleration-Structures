#ifndef SCENE_GEOMETRIES_H_
#define SCENE_GEOMETRIES_H_

#include "triangle.h"

// Very ugly solution for storing and loading multiple scenes
class sceneGeometries {
public:
	std::vector<triangle> baseScene;
	std::vector<triangle> testScene;

	// Reading .obj files guided by: https://vulkan-tutorial.com/Loading_models
	// Also by: https://github.com/canmom/rasteriser/blob/master/fileloader.cpp
	void createBaseScene() {
		std::cout << "Loading base scene started" << std::endl;
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
				prim.p0.y = 1.7f - prim.p0.y;
				prim.p1.y = 1.7f - prim.p1.y;
				prim.p2.y = 1.7f - prim.p2.y;
				prim.calculateCentroid();

				baseScene.push_back(prim);

				//// Triangle for second teapot
				//triangle prim2 = prim;
				//prim2.p0 += glm::vec3(0.0f, -4.0f, 0.0f);
				//prim2.p1 += glm::vec3(0.0f, -4.0f, 0.0f);
				//prim2.p2 += glm::vec3(0.0f, -4.0f, 0.0f);
				//prim2.calculateCentroid();

				//baseScene.push_back(prim2);
			}
		}
	}

	void createTestScene() {
		std::cout << "Loading test scene started" << std::endl;
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
				/*prim.p0.y = 1.7f - prim.p0.y;
				prim.p1.y = 1.7f - prim.p1.y;
				prim.p2.y = 1.7f - prim.p2.y;*/
				prim.calculateCentroid();

				testScene.push_back(prim);

				//// Triangle for second teapot
				//triangle prim2 = prim;
				//prim2.p0 += glm::vec3(0.0f, -4.0f, 0.0f);
				//prim2.p1 += glm::vec3(0.0f, -4.0f, 0.0f);
				//prim2.p2 += glm::vec3(0.0f, -4.0f, 0.0f);
				//prim2.calculateCentroid();

				//testScene.push_back(prim2);
			}
		}
	}
};

#endif // SCENE_GEOMETRIES_H_