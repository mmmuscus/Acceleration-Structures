#ifndef SCENE_GEOMETRIES_H_
#define SCENE_GEOMETRIES_H_

#include "triangle.h"

// Very ugly solution for storing and loading multiple scenes
class sceneGeometries {
private:
	std::vector<triangle> teapot;
	std::vector<triangle> sphere;
	std::vector<triangle> halfSphere;
	std::vector<triangle> spaceship;

public:
	std::vector<triangle> base;
	std::vector<triangle> lowOcclusionA;
	std::vector<triangle> lowOcclusionB;
	std::vector<triangle> highOcclusionA;
	std::vector<triangle> highOcclusionB;

	sceneGeometries() {
		loadTeapot();
		loadSphere();
		loadSpaceship();
		loadHalfSphere();
	}

	// Reading .obj files guided by: https://vulkan-tutorial.com/Loading_models
	// Also by: https://github.com/canmom/rasteriser/blob/master/fileloader.cpp
	void loadTeapot() {
		std::cout << "Loading teapot model" << std::endl;
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

				teapot.push_back(prim);
			}
		}
	}

	void loadSphere() {
		std::cout << "Loading sphere model" << std::endl;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		// Source of .obj: https://graphics.stanford.edu/courses/cs148-10-summer/as3/code/as3/teapot.obj
		std::string path = "assets/moon.obj";

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

				prim.p0.y += 0.1f;
				prim.p1.y += 0.1f;
				prim.p2.y += 0.1f;

				prim.calculateCentroid();

				sphere.push_back(prim);
			}
		}
	}

	void loadSpaceship() {
		std::cout << "Loading spaceship model" << std::endl;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		// Source of .obj: https://graphics.stanford.edu/courses/cs148-10-summer/as3/code/as3/teapot.obj
		std::string path = "assets/spaceship.obj";

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
			throw std::runtime_error(err);
		}

		std::vector<glm::vec3> vertices;

		// Load vertices
		for (const auto& shape : shapes) {
			const std::vector<tinyobj::index_t>& indices = shape.mesh.indices;
			const std::vector<int>& mat_ids = shape.mesh.material_ids;

			for (size_t face_ind = 0; face_ind < mat_ids.size() / 2; face_ind++) {
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

				prim.p0 *= -0.8f;
				prim.p1 *= -0.8f;
				prim.p2 *= -0.8f;

				prim.calculateCentroid();

				spaceship.push_back(prim);
			}
		}
	}

	void loadHalfSphere() {
		std::cout << "Loading half sphere model" << std::endl;
		for (int i = 0; i < sphere.size() / 2; i++) {
			triangle prim = sphere[i];
			float scaleFactor = 13.0f;

			prim.p0 *= scaleFactor;
			prim.p1 *= scaleFactor;
			prim.p2 *= scaleFactor;

			prim.calculateCentroid();

			halfSphere.push_back(prim);
		}
	}

	void createBaseScene() {
		std::cout << "Creating base scene" << std::endl;
		for (int i = 0; i < teapot.size(); i++) {
			base.push_back(teapot[i]);
		}
	}

	void createLowOcclusionSceneA() {
		std::cout << "Creating low occlusion A scene" << std::endl;
		int cubeSize = 6;
		float dist = 1.5f;
		glm::vec3 corner = glm::vec3(
			(dist / 2) + (cubeSize / -2.0f) * dist,
			(dist / 2) + (cubeSize / -2.0f) * dist,
			(dist / 2) + (cubeSize / -2.0f) * dist
		);

		std::vector<triangle> tempShpere;
		for (int n = 0; n < sphere.size(); n++) {
			triangle prim = sphere[n];
			float scaleFactor = 1.0f;

			prim.p0 *= scaleFactor;
			prim.p1 *= scaleFactor;
			prim.p2 *= scaleFactor;

			tempShpere.push_back(prim);
		}

		for (int i = 0; i < cubeSize; i++) {
			for (int j = 0; j < cubeSize; j++) {
				for (int k = 0; k < cubeSize; k++) {
					for (int n = 0; n < tempShpere.size(); n++) {
						triangle prim = tempShpere[n];

						prim.p0 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.p1 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.p2 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.calculateCentroid();

						lowOcclusionA.push_back(prim);
					}
				}
			}
		}
	}

	void createLowOcclusionSceneB() {
		std::cout << "Creating low occlusion B scene" << std::endl;
		for (int i = 0; i < spaceship.size(); i++) {
			lowOcclusionB.push_back(spaceship[i]);
		}
	}

	void createHighOcclusionSceneA() {
		std::cout << "Creating high occlusion A scene" << std::endl;
		for (int i = 0; i < halfSphere.size(); i++) {
			highOcclusionA.push_back(halfSphere[i]);
		}

		// Create sphere grid
		int cubeSize = 6;
		float dist = 10.0f;
		glm::vec3 corner = glm::vec3(
			(dist / 2) + (cubeSize / -2.0f) * dist,
			(dist / 2) + (cubeSize / -2.0f) * dist,
			(dist / 2) + (cubeSize / -2.0f) * dist
		);

		std::vector<triangle> tempShpere;
		for (int n = 0; n < sphere.size(); n++) {
			triangle prim = sphere[n];
			float scaleFactor = 8.0f;

			prim.p0 *= scaleFactor;
			prim.p1 *= scaleFactor;
			prim.p2 *= scaleFactor;

			tempShpere.push_back(prim);
		}

		for (int i = 0; i < cubeSize; i++) {
			for (int j = 0; j < cubeSize; j++) {
				for (int k = 0; k < cubeSize; k++) {
					for (int n = 0; n < tempShpere.size(); n++) {
						triangle prim = tempShpere[n];

						prim.p0 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.p1 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.p2 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.calculateCentroid();

						highOcclusionA.push_back(prim);
					}
				}
			}
		}
	}

	void createHighOcclusionSceneB() {
		std::cout << "Creating high occlusion B scene" << std::endl;
		for (int i = 0; i < halfSphere.size(); i++) {
			highOcclusionB.push_back(halfSphere[i]);
		}

		for (int i = 0; i < halfSphere.size(); i++) {
			triangle prim = halfSphere[i];
			float scaleFactor = -0.8f;

			prim.p0 *= scaleFactor;
			prim.p1 *= scaleFactor;
			prim.p2 *= scaleFactor;
			prim.calculateCentroid();

			highOcclusionB.push_back(prim);
		}

		// Create teapot grid
		int cubeSize = 3;
		float dist = 20.0f;
		glm::vec3 corner = glm::vec3(
			(dist / 2) + (cubeSize / -2.0f) * dist,
			(dist / 2) + (cubeSize / -2.0f) * dist,
			(dist / 2) + (cubeSize / -2.0f) * dist
		);

		for (int i = 0; i < cubeSize; i++) {
			for (int j = 0; j < cubeSize; j++) {
				for (int k = 0; k < cubeSize; k++) {
					for (int n = 0; n < teapot.size(); n++) {
						triangle prim = teapot[n];

						prim.p0 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.p1 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.p2 += glm::vec3(i * dist, j * dist, k * dist) + corner;
						prim.calculateCentroid();

						highOcclusionB.push_back(prim);
					}
				}
			}
		}
	}
};

#endif // SCENE_GEOMETRIES_H_