#ifndef RENDERER_H_
#define RENDERER_H_

#include "general.h"

class renderer {
private:


public:
	void createScreenTexture() {
        for (int j = 0; j < 255; j++) { // ROWS
            for (int i = 0; i < 255; i++) { // COLUMNS
                screenTexture[j][i][0] = (unsigned char)i;
                //screenTexture[j][i][0] = (unsigned char)(i * 255.0f / 720.0f);
                screenTexture[j][i][1] = (unsigned char)i;
                screenTexture[j][i][2] = (unsigned char)255;
            }
        }
	}

};

#endif // RENDERER_H_