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


    GLfloat* is_border;
public:
    GLfloat* velocity;
    GLfloat* pressure;
    fluid2DCpu(int width, int height, int pixelsPerCell, float fluidDensity);
    ~fluid2DCpu();
    void compute_gravity(float timeStep) const;
    void projection(int subStep, float timeStep, float o) const;
    void advection(float timeStep);
    [[nodiscard]] GLfloat find_max_pressure() const;
    [[nodiscard]] GLfloat find_min_pressure() const;



};


#endif //FLUIDSIM_FLUID2DCPU_H
