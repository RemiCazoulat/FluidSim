//
// Created by remi.cazoulat on 26/08/2024.
//



#ifndef FLUID_DENSITY_H
#define FLUID_DENSITY_H



#include "../../include/libraries.h"

enum DRAW_MODE {
    DENSITY,
    VELOCITY
};

class stable_fluid {
    int width;
    int height;
    int cell_size;
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

    stable_fluid(int width, int height, int cell_size, float diff, float visc);
    ~stable_fluid();

    void add_source(GLfloat *x, const GLfloat *s, float dt) const;
    void diffuse(int b, GLfloat *x, GLfloat *x0, float diff, float dt) const;
    void advect(int b, GLfloat *z, const GLfloat *z0, const GLfloat *u, const GLfloat *v, float dt) const;
    void project(GLfloat *u, GLfloat *v, GLfloat *p, GLfloat *div) const;
    void set_bound(int b, GLfloat *x) const;

    void watch_inputs(int mouse_pressed, float mouse_x, float mouse_y, float &force_x, float &force_y) const;
    void density_step(float dt);
    void velocity_step(float dt);


    void draw(DRAW_MODE mode) const;
    [[nodiscard]] GLfloat find_max(const GLfloat *x) const;
    [[nodiscard]] GLfloat find_min(const GLfloat* x) const;

};



#endif //FLUID_DENSITY_H
