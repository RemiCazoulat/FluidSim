//
// Created by Rémi on 15/09/2024.
//

#ifndef FLUIDSIM_GLFLUID2DOPTI_H
#define FLUIDSIM_GLFLUID2DOPTI_H


#include "Fluid2D.h"



class GlFluid2DOpti : public Fluid2D {
    // variables
    int width;
    int height;
    int cell_size;
    int sub_step;
    float grid_spacing;
    float diffusion;
    float viscosity;
    // arrays
    float* grid;
    float* dens_permanent;
    float* u_permanent;
    float* v_permanent;
    // compute programs
    GLuint stepsProgram;
    // textures
    GLuint grid_tex;
    GLuint dens_tex;
    GLuint dens_prev_tex;
    GLuint dens_perm_tex;
    GLuint pressure_tex;
    GLuint u_tex;
    GLuint v_tex;
    GLuint u_prev_tex;
    GLuint v_prev_tex;
    GLuint u_perm_tex;
    GLuint v_perm_tex;
    GLuint color_tex;

    void add_source(int x, int s, float dt);
    void add(int i, int j, float r, float intensity, int tex, float dt);
    void swap(int x_tex, int y_tex);
    void diffuse(int x_tex, int x0_tex, float diffusion_rate, float dt);
    void advect(int x_tex, int x0_tex, float dt);
    void project();
    void set_bounds_vel();
public:
    GlFluid2DOpti(int width, int height, int cell_size, float diff, float visc, int sub_step, float add_r, float add_i);
    ~GlFluid2DOpti() override;
    void input_step(float r, float* v_intensities, float dt) override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void pressure_step(float dt) override;
    [[nodiscard]] GLuint draw_step(DRAW_MODE mode) override;

    void debug() override;


};


#endif //FLUIDSIM_GLFLUID2DOPTI_H
