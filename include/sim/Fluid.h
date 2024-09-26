//
// Created by remi.cazoulat on 13/09/2024.
//



#ifndef FLUID_H
#define FLUID_H

#include "../../include/libraries.h"
#include "SimData.h"


class Fluid {
protected:
    ImGuiIO io;
    GLFWwindow* window;
public:
    SimData data;
    explicit Fluid(GLFWwindow* window);
    virtual ~Fluid() = default;
    virtual void input_step(float r, const float *intensities, float dt) = 0;
    virtual void density_step(float dt) = 0;
    virtual void velocity_step(float dt) = 0;
    virtual void pressure_step(float dt) = 0;
    virtual GLuint draw_step(DRAW_MODE mode) = 0;
    virtual void debug() = 0;

};



#endif //FLUID_H
