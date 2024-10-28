//
// Created by remi.cazoulat on 30/08/2024.
//

#ifndef CPUFLUID2D_H
#define CPUFLUID2D_H


#include "Fluid2D.h"

class cpuFlu2D final : public Fluid2D {

    float* grid;
    float* dens;
    float* dens_prev;
    float* dens_perm;
    float* pressure;
    float* u;
    float* v;
    float *u_permanent;
    float *v_perm;
    float* u_prev;
    float* v_prev;
    float* omega; // Vorticity
    float* gradu;
    float* gradv;
    float* cfu; // Confinement Force in u
    float* cfv; // Confinement Force in v
    float* color;

    void add_source(float *x, const float *s, float dt) const;
    void diffuse(float *x, const float *x0, float diffusion_rate, float dt) const;
    void advect(float *z, const float *z0, const float *u_vel, const float *v_vel, float dt) const;
    void project(float *p, float *div) const;
    void compute_vorticity();
    void compute_vorticity_grad();
    void compute_confinement_force(float epsilon);
    void apply_confinement_force();
    void confine(float epsilon);
    void add(int x, int y, float* t, float intensity) const;
    void set_vel_bound() const;

    [[nodiscard]] float find_max(const float* x) const;
    [[nodiscard]] float find_min(const float* x) const;

    void mouse_input(float dt) override;
    void sound_input(float dt) override;

public:
    cpuFlu2D(GLFWwindow* window, SimData* simData);
    ~cpuFlu2D() override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void pressure_step(float dt) override;
    GLuint draw_step(DRAW_MODE mode) override;
    void debug() override;
};


#endif //CPUFLUID2D_H
