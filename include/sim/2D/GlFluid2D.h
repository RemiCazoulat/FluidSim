//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef GLFLUID2D_H
#define GLFLUID2D_H

#include "Fluid2D.h"

class GlFluid2D final : public Fluid2D {


    // variables
    int sub_step;
    float grid_spacing;
    float diffusion;
    float viscosity;

    // debug variables
    double BINDING_TIME;
    double DISPATCH_TIME;

    double INPUT_STEP_TIME;
    double DENSITY_STEP_TIME;
    double VELOCITY_STEP_TIME;
    double PRESSURE_STEP_TIME;
    double DRAW_STEP_TIME;
    int TOTAL_STEPS;
    // arrays
    float* grid;
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
    void set_vel_bound();
    void add(int i, int j, float r, float intensity, GLuint tex, float dt);
    // utils
    void bind_and_run(const std::vector<GLuint> &textures, int how_many_times);
    void bind(const std::vector<GLuint> & textures);

public:

    // public and override methods
    GlFluid2D(int width, int height, int cell_size, float diff, float visc, int sub_step);
    ~GlFluid2D() override;
    void input_step(float r, const float* intensities, float dt) override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void pressure_step(float dt) override;
    [[nodiscard]] GLuint draw_step(DRAW_MODE mode) override;

    void debug() override;
};

#endif //GLFLUID2D_H
