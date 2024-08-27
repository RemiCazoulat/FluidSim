//
// Created by remi.cazoulat on 26/08/2024.
//

#ifndef FLUID_DENSITY_H
#define FLUID_DENSITY_H

#include "../../include/libraries.h"

class fluid_density {
    int width;
    int height;
    float grid_spacing;
    float diff;
    float visc;
public:
    GLfloat* is_b;
    GLfloat* dens;
    GLfloat* dens_prev;
    GLfloat* u;
    GLfloat* u_prev;
    GLfloat* v;
    GLfloat* v_prev;
    GLfloat* color;

    fluid_density(int width, int height, float diff, float visc);
    ~fluid_density();

    void add_source(GLfloat *x, const GLfloat *s, float dt) const;

    void diffuse(int b, GLfloat *x, GLfloat *x0, float diff, float dt) const;
    void advect(int b, GLfloat *z, const GLfloat *z0, const GLfloat *u, const GLfloat *v, float dt) const;

    void project(GLfloat *u, GLfloat *v, GLfloat *p, GLfloat *div) const;

    void density_step(float dt);

    void velocity_step(float dt);

    void set_color() const;

    [[nodiscard]] GLfloat find_max_dens() const;
    [[nodiscard]] GLfloat find_min_dens() const;

    void set_bound(int b, GLfloat *x) const;
};



#endif //FLUID_DENSITY_H
