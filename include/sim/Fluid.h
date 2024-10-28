//
// Created by remi.cazoulat on 13/09/2024.
//
#ifndef FLUID_H
#define FLUID_H

#include "../../include/libraries.h"
#include "FluidSim/include/SimData.h"

class Fluid {
protected:
    GLFWwindow* window;
    SimData* simData;
    virtual void mouse_input(float dt) = 0;
    virtual void sound_input(float dt) = 0;

public:
    explicit Fluid(GLFWwindow* window, SimData* simData);
    virtual ~Fluid() = default;
    void input_step(float dt);
    virtual void density_step(float dt) = 0;
    virtual void velocity_step(float dt) = 0;
    virtual void pressure_step(float dt) = 0;
    virtual GLuint draw_step(DRAW_MODE mode) = 0;
    virtual void debug() = 0;

};



#endif //FLUID_H
