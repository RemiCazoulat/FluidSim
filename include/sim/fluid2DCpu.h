//
// Created by Rémi on 21/08/2024.
//

#ifndef FLUIDSIM_FLUID2DCPU_H
#define FLUIDSIM_FLUID2DCPU_H

#include "../../include/libraries.h"
#include <sstream>
#include <iomanip>

class fluid2DCpu {

    int width;
    int height;
    int pixelsPerCell;
    float fluid_density;






public:
    GLfloat* is_border;
    GLfloat* pressure;
    GLfloat* velocity;
    GLfloat* pressure_color;

    fluid2DCpu(int width, int height, int pixels_per_cell, float fluid_density);
    ~fluid2DCpu();
    void compute_gravity(float time_step) const;
    void projection(int sub_step, float time_step, float o) const;
    void advection(float time_step);
    void calculate_pressure_color() const;

    void print_pressure() const;

    [[nodiscard]] GLfloat find_max_pressure() const;
    [[nodiscard]] GLfloat find_min_pressure() const;



};


#endif //FLUIDSIM_FLUID2DCPU_H
