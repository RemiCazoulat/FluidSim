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
    /// variables
    GLFWwindow* window;
    int width;
    int height;
    int cell_size;
    int sub_step;
    float grid_spacing;
    float diff;
    float visc;
    /// compute programs
    GLuint addProgram;
    GLuint advectProgram;
    GLuint diffuseProgram;
    GLuint projectProgram;
    GLuint boundProgram;
    GLuint swapProgram;
    /// textures
    GLuint gridTex;
    GLuint densTex;
    GLuint dens_prevTex;
    GLuint dens_permanentTex;
    GLuint pressureTex;
    GLuint uTex;
    GLuint vTex;
    GLuint*u_permanentTex;
    GLuint*v_permanentTex;
    GLuint u_prevTex;
    GLuint v_prevTex;
    GLuint colorTex;
    /// private methods
    void add_source(GLuint x, GLuint s, float dt) const;
    void diffuse(int b, GLuint x, const GLuint x0, float diff, float dt) const;
    void advect(int b, GLuint z, const GLuint z0, const GLuint u_vel, const GLuint v_vel, float dt) const;
    void project(GLuint p, GLuint div) const;
    void add_dens(int x, int y) const;
    void add_vel(int x, int y, float u_intensity, float v_intensity) const;
    void add_permanent_dens(int x, int y, float radius) const;
    void add_permanent_vel(int x, int y, float u_intensity, float v_intensity) const;
    void add_all_permanent_step() const;


public:
    /// public and override methods
    fluGpu(int width, int height, int cell_size);
    ~fluGpu() override;
    void inputs_step(int r, float intensity) const override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void calculate_pressure(float dt) const override;
    [[nodiscard]] GLuint draw(DRAW_MODE mode) const override;




};



#endif //FLUID2D_H
