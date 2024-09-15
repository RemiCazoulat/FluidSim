//
// Created by Rémi on 15/09/2024.
//

#ifndef FLUIDSIM_GLFLUID2DOPTI_H
#define FLUIDSIM_GLFLUID2DOPTI_H


#include "Fluid2D.h"

enum COMPUTE_MODE {
    INPUT,
    SOURCE,
    SWAP,
    DIFFUSE,
    ADVECT,
    PROJECT,
    BOUND
};

enum TEXTURES {
    GRID_T,
    DENS_T,
    DENS_PREV_T,
    DENS_PERM_T,
    VEL_T,
    VEL_PREV_T,
    VEL_PERM_T
};

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
    GLuint dens_permanent_tex;
    GLuint pressure_tex;
    GLuint vel_tex;
    GLuint vel_prev_tex;
    GLuint vel_permanent_tex;
    GLuint color_tex;

    void add_source(TEXTURES x, TEXTURES s, float dt);
    void add(int i, int j, float r, float u_intensity, float v_intensity, TEXTURES tex, float dt);
    void swap(TEXTURES x, TEXTURES y) noexcept;
    void diffuse_vel(float dt);
    void advect_vel(float dt);
    void project_vel();
    void set_bounds_vel();
public:
    GlFluid2DOpti(int width, int height, int cell_size, float diff, float visc, int sub_step, float add_r, float add_i);
    ~GlFluid2DOpti() override;
    void input_step(float r, float intensity, float dt) override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void pressure_step(float dt) override;
    [[nodiscard]] GLuint draw_step(DRAW_MODE mode) override;

    void debug() override;


};


#endif //FLUIDSIM_GLFLUID2DOPTI_H
