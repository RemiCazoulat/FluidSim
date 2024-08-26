//
// Created by remi.cazoulat on 26/08/2024.
//

#ifndef FLUID_DENSITY_H
#define FLUID_DENSITY_H

#include "../../include/libraries.h"


class fluid_density {
    int width;
    int height;
    float density;
    float grid_spacing;
public:
    GLfloat* is_border;
    GLfloat* pressure;
    GLfloat* velocity;
    GLfloat* pressure_color;

    fluid_density(int width, int height, float fluid_density);
    ~fluid_density();
    void compute_gravity(float dt) const;
    void diffusion(int sub_step, float dt, float diff) const;
    void advection(float time_step);
    void clearing_divergence();
    void calculate_pressure_color() const;

    [[nodiscard]] GLfloat find_max_pressure() const;
    [[nodiscard]] GLfloat find_min_pressure() const;

    void set_bound(int b);
};



#endif //FLUID_DENSITY_H
