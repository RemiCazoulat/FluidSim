//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef GLFLUID2D_H
#define GLFLUID2D_H

#include "Fluid2D.h"

class GlFluid2D final : public Fluid2D {


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
    GLuint dens_perm_tex;
    GLuint pressure_tex;
    GLuint u_tex;
    GLuint v_tex;
    GLuint u_perm_tex;
    GLuint v_perm_tex;
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
    void add_input(int i, int j, float r, float intensity, GLuint tex, float dt);
    // utils
    void bind_and_run(const std::vector<GLuint> &textures, int how_many_times);
    void bind(const std::vector<GLuint> & textures);

public:

    // public and override methods
    GlFluid2D(GLFWwindow* window, SimData* simData);
    ~GlFluid2D() override;
    void input_step( float dt) override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void pressure_step(float dt) override;
    GLuint draw_step(DRAW_MODE mode) override;

    void debug() override;
};

#endif //GLFLUID2D_H
