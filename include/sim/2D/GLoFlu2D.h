//
// Created by Rémi on 15/09/2024.
//

#ifndef GLFLUID2DOPTI_H
#define GLFLUID2DOPTI_H


#include "Fluid2D.h"


// Trying to optimize OpenGL simulation by using only one compute shader.
// /!\ not working for now.
class GLoFlu2D : public Fluid2D {
    // arrays
    float* grid;
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

    GLint dt_loc;
    GLint x_tex_loc;
    GLint y_tex_loc;
    GLint mode_loc;
    GLint i_loc;
    GLint j_loc;
    GLint r_loc;
    GLint intensity_loc;
    GLint a_loc;
    GLint dtw_loc;
    GLint dth_loc;
    GLint width_loc;
    GLint height_loc;
    GLint grid_spacing_loc;
    GLint stage_loc;
    GLint draw_mode_loc;

    void add_source(int x, int s, float dt) const;
    void add_input(int i, int j, float r, float intensity, int tex, float dt) const;
    void swap(int x_tex, int y_tex) const;
    void diffuse(int x_tex, int x0_tex, float diffusion_rate, float dt) const;
    void advect(int x_tex, int x0_tex, float dt) const;
    void project() const;
    void set_bounds_vel() const;

    void mouse_input(float dt) override;
    void sound_input(float dt) override;
public:
    GLoFlu2D(GLFWwindow* window, SimData* simData);
    ~GLoFlu2D() override;
    void density_step(float dt) override;
    void velocity_step(float dt) override;
    void pressure_step(float dt) override;
    GLuint draw_step(DRAW_MODE mode) override;
    void debug() override;
};


#endif //GLFLUID2DOPTI_H
