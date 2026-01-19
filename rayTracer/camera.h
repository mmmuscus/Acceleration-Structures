#ifndef CAMERA_H_
#define CAMERA_H_

#include "../general/general.h"

class camera {
private:
	glm::vec3 ogCamera;
	glm::vec3 ogTopRight, ogTopLeft, ogBottomLeft;
	glm::vec3 pivot;

	glm::vec3 cam;
	glm::vec3 topRight, topLeft, bottomLeft; // from perspective of camera

public:
	glm::vec3 getCam() { return cam; }

	camera(glm::vec3 _c) : ogCamera(_c), pivot(glm::vec3(0.0f, 0.0f, 0.0f)),
		ogTopRight(glm::vec3(ogCamera.x + 1.0f, ogCamera.x + 1.0f, ogCamera.z + 4.0f)),
		ogTopLeft(glm::vec3(ogCamera.x - 1.0f, ogCamera.x + 1.0f, ogCamera.z + 4.0f)),
		ogBottomLeft(glm::vec3(ogCamera.x - 1.0f, ogCamera.x - 1.0f, ogCamera.z + 4.0f)),
		cam(ogCamera),
		topRight(ogTopRight), topLeft(ogTopLeft), bottomLeft(ogBottomLeft)
	{}

	// Guided by https://stackoverflow.com/questions/50473848/rotate-point-around-pivot-point-repeatedly
	// Only rotates coordinates x and z, y should not change for our use case
	void spin(float rad) {
		cam = spinPoint(rad, ogCamera);
		topRight = spinPoint(rad, ogTopRight);
		topLeft = spinPoint(rad, ogTopLeft);
		bottomLeft = spinPoint(rad, ogBottomLeft);
	}

	glm::vec3 spinPoint(float rad, glm::vec3 point) {
		float cosTheta = cos(rad);
		float sinTheta = sin(rad);

		float z = (cosTheta * (point.z - pivot.z) - sinTheta * (point.x - pivot.x) + pivot.z);
		float x = (sinTheta * (point.z - pivot.z) + cosTheta * (point.x - pivot.x) + pivot.x);

		return glm::vec3(x, point.y, z);
	}

	glm::vec3 getPixelWorldPos(int row, int col) {
		return topLeft +
			(bottomLeft - topLeft) * ((float)row / HEIGHT) +
			(topRight - topLeft) * ((float)col / WIDTH);
	}
};

#endif // CAMERA_H_