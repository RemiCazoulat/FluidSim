//
// Created by Rémi on 21/08/2024.
//

#ifndef FLUIDSIM_FLUID2DCPU_H
#define FLUIDSIM_FLUID2DCPU_H

#include "../../include/libraries.h"

class fluid2DCpu {

    int width;
    int height;
    int pixelsPerCell;
    float fluid_density;

    GLfloat* vel;
    GLfloat* pressure;
    GLfloat* is_border;
public:
    fluid2DCpu(int width, int height, int pixelsPerCell, float fluidDensity);
    ~fluid2DCpu();
    void compute_gravity(float timeStep) const;
    void projection(int subStep, float timeStep, float o) const;
    void advection(float timeStep);
    GLfloat* get_vel();
    GLfloat* get_dens();


};


#endif //FLUIDSIM_FLUID2DCPU_H
