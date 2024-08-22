//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef FLUID2D_H
#define FLUID2D_H

#include "../../include/libraries.h"
#include "../../include/shaders/compute.h"
#include "../../include/shaders/shader.h"

GLuint createTextureVec2(const GLfloat * data, int width, int height);
GLuint createTextureVec1(const GLfloat * data, int width, int height);



class fluid2DGpu {
    GLuint velocityTex;
    GLuint pressureTex;

    GLuint projectionProgram;
    GLuint combineProjProgram;

    GLfloat* vel;
    GLfloat* pressure;
    GLfloat* grid;
    GLfloat* results;

    int width;
    int heigth;
    int pixelsPerCell;
    float fluidDensity;

public:
    fluid2DGpu(int width, int heigth, int pixelsPerCell, float fluidDensity);

};



#endif //FLUID2D_H
