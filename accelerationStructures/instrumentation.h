#ifndef INSTRUMENTATION_H_
#define INSTRUMENTATION_H_

#include "../general/general.h"

struct step {
	unsigned int intersectionTests;
	unsigned int traversalSteps; 
};

struct floatStep {
	float intersectionTests;
	float traversalSteps;
};

int angle;

struct measurements {
	step min, max;
	floatStep avg;
};

measurements m[ANGLES];

void printMTraversalCSV() {
	std::cout << "Traversal steps per angle: " << std::endl;
	for (int i = 0; i < ANGLES; i++) {
		std::cout << i << ", "
			<< m[i].min.traversalSteps << ", "
			<< m[i].avg.traversalSteps << ", "
			<< m[i].max.traversalSteps
			<< std::endl;
	}
}

void printMIntersectionCSV() {
	std::cout << "Intersection tests per angle: " << std::endl;
	for (int i = 0; i < ANGLES; i++) {
		std::cout << i << ", "
			<< m[i].min.intersectionTests << ", "
			<< m[i].avg.intersectionTests << ", "
			<< m[i].max.intersectionTests
			<< std::endl;
	}
}

void printMCSV() {
	printMIntersectionCSV();
	printMTraversalCSV();
}

void printBVHEval() {
	float tAvgAvg = 0;
	float tMaxAvg = 0;
	float iAvgAvg = 0;
	float iMaxAvg = 0;

	float tAvgMax = 0;
	int tMaxMax = 0;
	float iAvgMax = 0;
	int iMaxMax = 0;

	for (int i = 0; i < ANGLES; i++) {
		tAvgAvg += m[i].avg.traversalSteps;
		tMaxAvg += m[i].max.traversalSteps;
		iAvgAvg += m[i].avg.intersectionTests;
		iMaxAvg += m[i].max.intersectionTests;

		if (tAvgMax < m[i].avg.traversalSteps)
			tAvgMax = m[i].avg.traversalSteps;

		if (tMaxMax < m[i].max.traversalSteps)
			tMaxMax = m[i].max.traversalSteps;

		if (iAvgMax < m[i].avg.intersectionTests)
			iAvgMax = m[i].avg.intersectionTests;

		if (iMaxMax < m[i].max.intersectionTests)
			iMaxMax = m[i].max.intersectionTests;
	}

	tAvgAvg /= (float)ANGLES;
	tMaxAvg /= (float)ANGLES;
	iAvgAvg /= (float)ANGLES;
	iMaxAvg /= (float)ANGLES;

	std::cout << "Intersection: " << std::endl;
	printf("AoAI: %.2f\n", iAvgAvg);
	printf("AoMI: %.2f\n", iMaxAvg);
	printf("MoAI: %.2f\n", iAvgMax);
	printf("MoMI: %d\n", iMaxMax);
	printf("AoAT: %.2f\n", tAvgAvg);
	printf("MoAT: %.2f\n", tMaxAvg);
	printf("AoMT: %.2f\n", tAvgMax);
	printf("MoMT: %d\n\n", tMaxMax);
}

class instrumentation {
private:
	unsigned int currentHeight;
	unsigned int currentWidth;

	step steps[HEIGHT][WIDTH];
	step min, max;
	floatStep avgSteps[HEIGHT][WIDTH];
	floatStep avg;

	glm::vec3 startColor;
	glm::vec3 endColor;

public:
	instrumentation() : currentHeight(0), currentWidth(0),
		min(0), avg(0.0f), max(0),
		startColor(glm::vec3( // Used when min value
			0.0f, 0.0f, 0.0f
		)),
		endColor(glm::vec3( // Used when max value
			255.0f, 255.0f, 255.0f
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

		avg.traversalSteps = (float)traversalStepSum / (float)(WIDTH * HEIGHT);
		avg.intersectionTests = (float)intersectionTestsSum / (float)(WIDTH * HEIGHT);

		// Update plot values
		m[angle].min = min;
		m[angle].avg = avg;
		m[angle].max = max;
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
				textures[1][offset][0] = lerpedColor.x;
				textures[1][offset][1] = lerpedColor.y;
				textures[1][offset][2] = lerpedColor.z;

				traversalValue /= diffTraversal;
				lerpedColor = findLerpValue(traversalValue);
				textures[2][offset][0] = lerpedColor.x;
				textures[2][offset][1] = lerpedColor.y;
				textures[2][offset][2] = lerpedColor.z;


				combinedValue /=  diffCombined;
				lerpedColor = findLerpValue(combinedValue);
				textures[3][offset][0] = 
					startColor.y * (1.0f - intersectionValue) + endColor.y * intersectionValue;
				textures[3][offset][1] = 
					startColor.x * (1.0f - traversalValue) + endColor.x * traversalValue;
				textures[3][offset][2] = 0;
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
