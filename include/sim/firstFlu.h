//
// Created by Rémi on 21/08/2024.
//

#ifndef FLUIDSIM_FIRSTFLU_H
#define FLUIDSIM_FIRSTFLU_H

#include "../../include/libraries.h"


class firstFlu {

    int width;
    int height;
    float fluid_density;
    float grid_spacing;

public:
    GLfloat* is_border;
    GLfloat* pressure;
    GLfloat* velocity;
    GLfloat* pressure_color;

    firstFlu(int width, int height, float fluid_density);
    ~firstFlu();
    void compute_gravity(float time_step) const;
    void projection(int sub_step, float time_step, float o) const;
    void advection(float time_step);
    void calculate_pressure_color() const;

    void print_pressure() const;

    [[nodiscard]] GLfloat find_max_pressure() const;
    [[nodiscard]] GLfloat find_min_pressure() const;



};


#endif //FLUIDSIM_FIRSTFLU_H
