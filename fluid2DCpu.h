//
// Created by Rémi on 21/08/2024.
//

#ifndef FLUIDSIM_FLUID2DCPU_H
#define FLUIDSIM_FLUID2DCPU_H

#include "../../include/libraries.h"

class fluid2DCpu {

    uint width;
    uint height;

    GLfloat[] vel;
    GLfloat[] dens;
    GLfloat[] is_border;
public:
    fluid2DCpu(const float width, const float height);
    ~fluid2DCpu();
    void compute_gravity(const float timeStep)
    void projection(const int subStep, const float o);
    void advection(const int timeStep);
    GLfloat* get_vel();
    GLfloat* get_dens();


};


#endif //FLUIDSIM_FLUID2DCPU_H
