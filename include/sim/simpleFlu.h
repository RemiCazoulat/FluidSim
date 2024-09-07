//
// Created by remi.cazoulat on 30/08/2024.
//

#ifndef FLUSIM2DCPU2_H
#define FLUSIM2DCPU2_H



#include "../../include/libraries.h"
#include "fluid.h"

class simpleFlu final : public fluid {
    GLFWwindow* window;
    int width;
    int height;
    int cell_size;
    int sub_step;
    float grid_spacing;
    float diff;
    float visc;

    float* is_b;
    float* dens;
    float* dens_prev;
    float* dens_permanent;
    float* pressure;
    float* u;
    float* v;
    float *u_permanent;
    float *v_permanent;
    float* u_prev;
    float* v_prev;
    float* color;

    void add_source(float *x, const float *s, float dt) const;
    void diffuse(int b, float *x, const float *x0, float diff, float dt) const;
    float interpolate(float x, float y, const float *t, float dx, float dy) const;
    void advect_vel(float dt) const;
    void advect(int b, float *z, const float *z0, const float *u_vel, const float *v_vel, float dt) const;

    void project_simple() const;

    void project(float *u, float *v, float *p, float *div) const;
    void set_bound(int b, float *x) const;
    void add_dens(int x, int y) const;
    void add_permanent_dens(int x, int y, float radius) const;
    void add_vel(int x, int y, float u_intensity, float v_intensity) const;
    void add_permanent_vel(int x, int y, float u_intensity, float v_intensity) const;
    void add_all_perm_step() const;

    [[nodiscard]] float find_max(const float* x) const;
    [[nodiscard]] float find_min(const float* x) const;
public:

    simpleFlu(GLFWwindow* window, int width, int height, int cell_size, float diff, float visc, int sub_step);
    ~simpleFlu() override;
    void inputs_step(int r, float intensity) const override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void calculate_pressure(float dt) const override;
    [[nodiscard]] float* draw(DRAW_MODE mode) const override;

    void set_vel_bound() const;
};


#endif //FLUSIM2DCPU2_H
