#ifndef INSTRUMENTATION_H_
#define INSTRUMENTATION_H_

#include "../general/general.h"

struct step {
	unsigned int intersectionTests;
	unsigned int traversalSteps; 
};

class instrumentation {
private:
	unsigned int currentHeight;
	unsigned int currentWidth;

	step steps[HEIGHT][WIDTH];
	step min, avg, max;

	glm::vec3 startColor;
	glm::vec3 endColor;

public:
	instrumentation() : currentHeight(0), currentWidth(0),
		min(0), avg(0), max(0),
		startColor(glm::vec3( // Used when min value
			56.0f, 6.0f, 89.0f
		)),
		endColor(glm::vec3( // Used when max value
			252.0f, 101.0f, 13.0f
		))
	{
		reset();
	}

	void setHeightWidth(unsigned int h, unsigned int w) {
		currentHeight = h;
		currentWidth = w;
	}

	void increaseIntersection() {
		steps[currentHeight][currentWidth].intersectionTests++;
	}

	void increaseTraversal() {
		steps[currentHeight][currentWidth].traversalSteps++;
	}

	void reset() {
		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				steps[i][j].intersectionTests = 0;
				steps[i][j].traversalSteps = 0;
			}
		}
	}

	void calculateValues() {
		max.traversalSteps = steps[0][0].traversalSteps;
		max.intersectionTests = steps[0][0].intersectionTests;
		min.traversalSteps = steps[0][0].traversalSteps;
		min.intersectionTests = steps[0][0].intersectionTests;

		unsigned int traversalStepSum = 0;
		unsigned int intersectionTestsSum = 0;

		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				traversalStepSum += steps[i][j].traversalSteps;
				intersectionTestsSum += steps[i][j].intersectionTests;

				if (steps[i][j].traversalSteps > max.traversalSteps) 
					max.traversalSteps = steps[i][j].traversalSteps;
				else if (steps[i][j].traversalSteps < min.traversalSteps)
					min.traversalSteps = steps[i][j].traversalSteps;

				if (steps[i][j].intersectionTests > max.intersectionTests)
					max.intersectionTests = steps[i][j].intersectionTests;
				else if (steps[i][j].intersectionTests < min.intersectionTests)
					min.intersectionTests = steps[i][j].intersectionTests;
			}
		}

		avg.traversalSteps = traversalStepSum / (WIDTH * HEIGHT);
		avg.intersectionTests = intersectionTestsSum / (WIDTH * HEIGHT);
	}
	void printIntersection() {
		std::cout << "Intersection tests MAX: " << max.intersectionTests << std::endl;
		std::cout << "Intersection tests AVG: " << avg.intersectionTests << std::endl;
		std::cout << "Intersection tests MIN: " << min.intersectionTests << std::endl;
	}

	void printTraversal() {
		std::cout << "Traversal steps MAX: " << max.traversalSteps << std::endl;
		std::cout << "Traversal steps AVG: " << avg.traversalSteps << std::endl;
		std::cout << "Traversal steps MIN: " << min.traversalSteps << std::endl;
	}

	void print() {
		printIntersection();
		printTraversal();
	}

	void createTextures() {
		unsigned int diffIntersection = max.intersectionTests - min.intersectionTests;
		unsigned int diffTraversal = max.traversalSteps - min.traversalSteps;
		unsigned int diffCombined = diffIntersection + diffTraversal;
		for (int j = 0; j < HEIGHT; j++) { // ROWS
			for (int i = 0; i < WIDTH; i++) { // COLUMNS
				unsigned int offset = j * WIDTH + i;

				float intersectionValue = 
					steps[j][i].intersectionTests - min.intersectionTests;
				float traversalValue =
					steps[j][i].traversalSteps - min.traversalSteps;
				float combinedValue = intersectionValue + traversalValue;

				intersectionValue /= diffIntersection;
				glm::vec3 lerpedColor = findLerpValue(intersectionValue);
				intersectionTexture[offset][0] = lerpedColor.x;
				intersectionTexture[offset][1] = lerpedColor.y;
				intersectionTexture[offset][2] = lerpedColor.z;

				traversalValue /= diffTraversal;
				lerpedColor = findLerpValue(traversalValue);
				traversalTexture[offset][0] = lerpedColor.x;
				traversalTexture[offset][1] = lerpedColor.y;
				traversalTexture[offset][2] = lerpedColor.z;

				combinedValue /=  diffCombined;
				lerpedColor = findLerpValue(combinedValue);
				combinedTexture[offset][0] = lerpedColor.x;
				combinedTexture[offset][1] = lerpedColor.y;
				combinedTexture[offset][2] = lerpedColor.z;
			}
		}
	}

	glm::vec3 findLerpValue(float val) {
		return glm::vec3(
			startColor * (1.0f - val) + endColor * val
		);
	}
};

instrumentation stepCounter;

#endif // !INSTRUMENTATION_H_
