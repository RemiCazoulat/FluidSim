//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef FLUID2D_H
#define FLUID2D_H

#include "../../include/libraries.h"

class fluid2DGpu {
    GLuint velocityTex;
    GLuint pressureTex;

    GLuint projectionProgram;
    GLuint combineProjProgram;

    GLfloat* vel;
    GLfloat* dens;
    GLfloat* grid;
    GLfloat* results;

    int width;
    int heigth;
    int pixelsPerCell;

public:
    fluid2DGpu(int width, int heigth, int pixelsPerCell, float fluidDensity);

};



#endif //FLUID2D_H
