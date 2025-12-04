#ifndef RENDERER_H_
#define RENDERER_H_

#include "general.h"

class renderer {
private:


public:
	void createScreenTexture() {
        for (int j = 0; j < HEIGHT; j++) { // ROWS
            for (int i = 0; i < WIDTH; i++) { // COLUMNS
                unsigned int offset = j * WIDTH + i;
                screenTexture[offset][0] = i;
                screenTexture[offset][1] = j;
                screenTexture[offset][2] = 0;
            }
        }
	}

};

#endif // RENDERER_H_