//
// Created by remi.cazoulat on 30/08/2024.
//

#ifndef CPUFLUID2D_H
#define CPUFLUID2D_H


#include "Fluid2D.h"

class CpuFluid2D final : public Fluid2D {
    int sub_step;
    float grid_spacing;
    float diff;
    float visc;

    float* grid;
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
    void diffuse(float *x, const float *x0, float diffusion_rate, float dt) const;
    void advect(float *z, const float *z0, const float *u_vel, const float *v_vel, float dt) const;
    void project(float *p, float *div) const;
    void add(int x, int y, float* t, float intensity) const;
    void set_vel_bound() const;

    [[nodiscard]] float find_max(const float* x) const;
    [[nodiscard]] float find_min(const float* x) const;
public:
    CpuFluid2D(int width, int height, int cell_size, float diff, float visc, int sub_step);
    ~CpuFluid2D() override;
    void input_step(float r, const float* intensities, float dt) override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void pressure_step(float dt) override;
    [[nodiscard]] GLuint draw_step(DRAW_MODE mode) override;
    void debug() override;
};


#endif //CPUFLUID2D_H
