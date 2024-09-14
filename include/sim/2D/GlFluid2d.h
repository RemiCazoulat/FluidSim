//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef FLUID2D_H
#define FLUID2D_H

#include "Fluid2d.h"

class GlFluid2d final : public Fluid2d {


    // variables
    int width;
    int height;
    int cell_size;
    int sub_step;
    float grid_spacing;
    float diffusion;
    float viscosity;

    // debug variables
    double BINDING_TIME;
    double UNBINDING_TIME;
    double DISPATCH_TIME;

    double INPUT_STEP_TIME;
    double DENSITY_STEP_TIME;
    double VELOCITY_STEP_TIME;
    double PRESSURE_STEP_TIME;
    double DRAW_STEP_TIME;
    int TOTAL_STEPS;
    // arrays
    float* grid;
    float* dens_permanent;
    float* u_permanent;
    float* v_permanent;

    // compute programs
    GLuint inputProgram;
    GLuint addProgram;
    GLuint advectProgram;
    GLuint diffuseProgram;
    GLuint projectProgram;
    GLuint boundProgram;
    GLuint drawProgram;

    // textures
    GLuint grid_tex;
    GLuint dens_tex;
    GLuint dens_prev_tex;
    GLuint dens_permanent_tex;
    GLuint pressure_tex;
    GLuint u_tex;
    GLuint v_tex;
    GLuint u_permanent_tex;
    GLuint v_permanent_tex;
    GLuint u_prev_tex;
    GLuint v_prev_tex;
    GLuint color_tex;

    // private methods
    void add_source(GLuint x, GLuint s, float dt);

    static void swap(GLuint &x, GLuint &y) noexcept;
    void diffuse(GLuint x, GLuint x0, float diff, float dt);
    void advect( GLuint z, GLuint z0, float dt);
    void project();
    void add(int i, int j, float r, float intensity, GLuint tex, float dt);
    void set_vel_bound();

public:

    // public and override methods
    GlFluid2d(int width, int height, int cell_size, float diff, float visc, int sub_step, float add_r, float add_i);
    ~GlFluid2d() override;
    void input_step(float r, float intensity, float dt) override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void pressure_step(float dt) override;
    [[nodiscard]] GLuint draw_step(DRAW_MODE mode) override;

    void debug() override;
};

#endif //FLUID2D_H
