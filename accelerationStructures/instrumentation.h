#ifndef INSTRUMENTATION_H_
#define INSTRUMENTATION_H_

#include "../general/general.h"

struct step {
	unsigned int intersectionTests;
	unsigned int traversalSteps; 
};

class instrumentation {
private:
	unsigned int intersectionTests;
	unsigned int traversalSteps;

public:
	instrumentation() : intersectionTests(0), traversalSteps(0) {}

	void increaseIntersection() {
		intersectionTests++;
	}

	void increaseTraversal() {
		traversalSteps++;
	}

	void reset() {
		intersectionTests = 0;
		traversalSteps = 0;
	}

	void printIntersection() {
		std::cout << "Number of intersection tests: " << intersectionTests << std::endl;
	}

	void printTraversal() {
		std::cout << "Number of traversal steps: " << traversalSteps << std::endl;
	}

	void print() {
		printIntersection();
		printTraversal();
	}
};

instrumentation stepCounter;

#endif // !INSTRUMENTATION_H_
