#ifndef RAY_H_
#define RAY_H_

#include "../general/general.h"

class ray {
public:
	glm::vec3 O, D;
	float t = 1e30f;
};

#endif // RAY_H_