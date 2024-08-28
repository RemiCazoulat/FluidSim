//
// Created by remi.cazoulat on 26/08/2024.
//



#ifndef FLUID_DENSITY_H
#define FLUID_DENSITY_H



#include "../../include/libraries.h"

enum DRAW_MODE {
    DENSITY,
    VELOCITY,
    MIXED
};

class stable_fluid {
    int width;
    int height;
    int cell_size;
    int sub_step;
    float grid_spacing;
    float diff;
    float visc;
public:
    float* is_b;
    float* dens;
    float* dens_prev;
    float* dens_permanent;
    float* u;
    float* v;
    float *u_permanent;
    float *v_permanent;
    float* u_prev;
    float* v_prev;
    float* color;

    stable_fluid(int width, int height, int cell_size, float diff, float visc, int sub_step);
    ~stable_fluid();

    void add_source(float *x, const float *s, float dt) const;
    void diffuse(int b, float *x, const float *x0, float diff, float dt) const;
    void advect(int b, float *z, const float *z0, const float *u, const float *v, float dt) const;
    void project(float *u, float *v, float *p, float *div) const;
    void set_bound(int b, float *x) const;

    void density_step(float dt);
    void velocity_step(float dt);

    void add_dens(int x, int y) const;
    void add_permanent_dens(int x, int y, float radius) const;
    void add_vel(int x, int y, float u_intensity, float v_intensity) const;
    void add_permanent_vel(int x, int y, float u_intensity, float v_intensity) const;
    void add_all_perm_step() const;


    void draw(DRAW_MODE mode) const;
    [[nodiscard]] float find_max(const float* x) const;
    [[nodiscard]] float find_min(const float* x) const;

};



#endif //FLUID_DENSITY_H
