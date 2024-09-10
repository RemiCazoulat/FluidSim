//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef FLUID2D_H
#define FLUID2D_H

#include "../../include/libraries.h"
#include "../../include/shaders/compute.h"
#include "../../include/shaders/shader.h"
#include "fluid.h"


class fluGpu final : public fluid {

    // variables
    GLFWwindow* window;
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
    void add_source(GLuint x, GLuint s, float dt) const;

    static void swap(GLuint &x, GLuint &y) noexcept;
    void diffuse(GLuint x, GLuint x0, float diff, float dt) const;
    void advect(GLuint z, GLuint z0, GLuint u_vel, GLuint v_vel, float dt) const;
    void project(GLuint p, GLuint div) const;
    void add(int x, int y, float* t, float intensity) const;
    void set_vel_bound() const;

public:

    // public and override methods
    fluGpu(GLFWwindow* window, int width, int height, int cell_size, float diff, float visc, int sub_step);
    ~fluGpu() override;
    void input_step(int r, float intensity, float dt) override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void calculate_pressure(float dt) const override;
    [[nodiscard]] GLuint draw(DRAW_MODE mode) const override;
};

#endif //FLUID2D_H
