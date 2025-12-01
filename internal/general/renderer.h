#ifndef RENDERER_H_
#define RENDERER_H_

#include "general.h"

class renderer {
private:


public:
	void createScreenTexture() {
        for (int j = 0; j < HEIGHT; j++) {
            for (int i = 0; i < WIDTH; i++) {
                screenTexture[i][j][0] = (unsigned char)0;
                screenTexture[i][j][0] = (unsigned char)(i * 255.0f / 1280.0f);
                //screenTexture[i][j][0] = (unsigned char)(i * 255.0f / 720.0f);
                screenTexture[i][j][1] = (unsigned char)0;
                screenTexture[i][j][2] = (unsigned char)0;
            }
        }
	}

};

#endif // RENDERER_H_