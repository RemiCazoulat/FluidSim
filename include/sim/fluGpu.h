//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef FLUID2D_H
#define FLUID2D_H

#include "../../include/libraries.h"
#include "../../include/shaders/compute.h"
#include "../../include/shaders/shader.h"





class fluGpu {


    GLuint projectionProgram;
    GLuint combineProjProgram;

    GLfloat* velocity;
    GLfloat* pressure;
    GLfloat* grid;
    GLfloat* results;

    int width;
    int heigth;
    int pixelsPerCell;
    float fluidDensity;

public:
    GLuint velocityTex;
    GLuint pressureTex;

    fluGpu(int width, int heigth, int pixelsPerCell, float fluidDensity);
    ~fluGpu();
    void projection(int subStep, float timeStep) const;

};



#endif //FLUID2D_H
