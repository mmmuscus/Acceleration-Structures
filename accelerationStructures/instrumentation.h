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

public:
	step steps[HEIGHT][WIDTH];
	step min, avg, max;

	instrumentation() : currentHeight(0), currentWidth(0),
		min(0), avg(0), max(0)
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
		std::cout << "Intersection test measurements: " << std::endl;
		std::cout << "MAX: " << max.intersectionTests << std::endl;
		std::cout << "AVG: " << avg.intersectionTests << std::endl;
		std::cout << "MIN: " << min.intersectionTests << std::endl;
	}

	void printTraversal() {
		std::cout << "Traversal step measurements: " << std::endl;
		std::cout << "MAX: " << max.traversalSteps << std::endl;
		std::cout << "AVG: " << avg.traversalSteps << std::endl;
		std::cout << "MIN: " << min.traversalSteps << std::endl;
	}

	void print() {
		printIntersection();
		printTraversal();
	}
};

instrumentation stepCounter;

#endif // !INSTRUMENTATION_H_
