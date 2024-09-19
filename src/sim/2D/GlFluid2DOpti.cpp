//
// Created by Rémi on 15/09/2024.
//

#include "../../../include/sim/2D/GlFluid2DOpti.h"
#include "../../../include/shaders/compute.h"

// macros for textures
#define GRID_T      0
#define DENS_T      1
#define DENS_PREV_T 2
#define DENS_PERM_T 3
#define PRESSURE    11
#define U_T         4
#define V_T         5
#define U_PREV_T    6
#define V_PREV_T    7
#define U_PERM_T    8
#define V_PERM_T    9
#define COLOR_T     10
// macros for modes
#define INPUT     0
#define SOURCE    1
#define SWAP      2
#define DIFFUSE   3
#define ADVECT    4
#define PROJECT   5
#define BOUND_D   6
#define BOUND_V   7
#define DRAW      8

GlFluid2DOpti::GlFluid2DOpti(const int width, const int height, const int cell_size, const float diff, const float visc, const int sub_step)
: Fluid2D(width, height, cell_size)
{
    // ----------{ init variables }----------
    this->diffusion = diff;
    this->viscosity = visc;
    this->sub_step = sub_step;
    this->grid_spacing = 1.f / static_cast<float>(height);
    const int grid_size = width * height;
    grid = new float[grid_size]();
    const auto* empty = new float[grid_size]();
    const auto* emptyVec4 = new float[grid_size * 4]();
    constexpr float r = 10.f;
    const int circle_x = width / 2;
    const int circle_y = height / 2;
    for (int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = i + jw;
            if (i == 0 || j == 0 || i == width - 1 || j == height - 1) grid[index] = 0.0;
            else grid[index] = 1.0;
            const int dist_x = circle_x - i;
            const int dist_y = circle_y - j;
            if(std::sqrt(dist_x * dist_x + dist_y * dist_y) < r) grid[index] = 0.0;
        }
    }
    // ----------{ Init Textures }----------
    grid_tex           = createTextureVec1(grid, width, height);
    dens_tex           = createTextureVec1(empty, width, height);
    dens_prev_tex      = createTextureVec1(empty, width, height);
    dens_perm_tex      = createTextureVec1(empty, width, height);
    pressure_tex       = createTextureVec1(empty, width, height);
    u_tex              = createTextureVec1(empty, width, height);
    v_tex              = createTextureVec1(empty, width, height);
    u_prev_tex         = createTextureVec1(empty, width, height);
    v_prev_tex         = createTextureVec1(empty, width, height);
    u_perm_tex         = createTextureVec1(empty, width, height);
    v_perm_tex         = createTextureVec1(empty, width, height);
    color_tex          = createTextureVec4(emptyVec4, width, height);
    // ---------- { Compute programs }----------
    stepsProgram = createComputeProgram("../shaders/computes/steps.glsl");
    glUseProgram(stepsProgram);
    glBindImageTexture(0, grid_tex     , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, dens_tex     , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, dens_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, dens_perm_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, pressure_tex , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(5, u_tex        , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(6, v_tex        , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(7, u_prev_tex   , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(8, v_prev_tex   , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(9, u_perm_tex   , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(10, v_perm_tex  , 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(11, color_tex   , 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    dt_loc           = glGetUniformLocation(stepsProgram, "dt"          ); // time
    x_tex_loc        = glGetUniformLocation(stepsProgram, "x_tex"       ); // which textures use
    y_tex_loc        = glGetUniformLocation(stepsProgram, "y_tex"       );
    mode_loc         = glGetUniformLocation(stepsProgram, "mode"        ); // which function call
    i_loc            = glGetUniformLocation(stepsProgram, "i"           );
    j_loc            = glGetUniformLocation(stepsProgram, "j"           );
    r_loc            = glGetUniformLocation(stepsProgram, "r"           ); // for add
    intensity_loc    = glGetUniformLocation(stepsProgram, "intensity"   );
    a_loc            = glGetUniformLocation(stepsProgram, "a"           ); // for diffuse
    dtw_loc          = glGetUniformLocation(stepsProgram, "dtw"         ); // for advect
    dth_loc          = glGetUniformLocation(stepsProgram, "dth"         );
    width_loc        = glGetUniformLocation(stepsProgram, "width"       );
    height_loc       = glGetUniformLocation(stepsProgram, "height"      );
    grid_spacing_loc = glGetUniformLocation(stepsProgram, "grid_spacing"); // for project
    stage_loc        = glGetUniformLocation(stepsProgram, "stage"       );
    draw_mode_loc    = glGetUniformLocation(stepsProgram, "draw_mode"   ); // for draw

    delete[] empty;
    delete[] emptyVec4;
}

GlFluid2DOpti::~GlFluid2DOpti() {
    delete[] grid;
    glDeleteTextures(1, &grid_tex);
    glDeleteTextures(1, &dens_tex);
    glDeleteTextures(1, &dens_prev_tex);
    glDeleteTextures(1, &dens_perm_tex);
    glDeleteTextures(1, &pressure_tex);
    glDeleteTextures(1, &u_tex);
    glDeleteTextures(1, &v_tex);
    glDeleteTextures(1, &u_prev_tex);
    glDeleteTextures(1, &v_prev_tex);
    glDeleteTextures(1, &u_perm_tex);
    glDeleteTextures(1, &v_perm_tex);
    glDeleteTextures(1, &color_tex);
    glDeleteProgram(stepsProgram);
}

void GlFluid2DOpti::add(const int i,const int j ,const float r ,const float intensity, const int tex, const float dt) const {
    glUniform1i(mode_loc, INPUT);
    glUniform1i(i_loc, i);
    glUniform1i(j_loc, j);
    glUniform1f(r_loc, r);
    glUniform1f(intensity_loc, intensity);
    glUniform1i(x_tex_loc, tex);
    glUniform1f(dt_loc, dt);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    printf("add finished. \n");
}

void GlFluid2DOpti::input_step(const float r, const float* intensities, const float dt) {
    if (left_mouse_pressed || right_mouse_pressed || middle_mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / cell_size;
        const int j = static_cast<int>((static_cast<float>(cell_size * height) - mouse_y)) / cell_size;

        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            if(left_mouse_pressed || middle_mouse_pressed) {
                if (left_mouse_pressed){
                    add(i, j, r, (mouse_x - force_x), U_T, dt);
                    add(i, j, r, -(mouse_y - force_y),V_T, dt);

                }
                if(middle_mouse_pressed) {
                    add(i, j, r, intensities[0], U_PERM_T, dt);
                    add(i, j, r, intensities[1], V_PERM_T, dt);
                }
            }
        }
        force_x = mouse_x;
        force_y = mouse_y;
    }
    add_source(DENS_T, DENS_PERM_T, dt);
    add_source(U_T, U_PERM_T, dt);
    add_source(V_T, V_PERM_T, dt);
}

void GlFluid2DOpti::add_source(const int x, const int s, float dt) const {
    glUniform1i(mode_loc, SOURCE);
    glUniform1i(x_tex_loc, x);
    glUniform1i(y_tex_loc, s);
    glUniform1f(dt_loc, dt);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void GlFluid2DOpti::swap(const int x, const int y) const {
    glUniform1i(mode_loc, SWAP);
    glUniform1i(x_tex_loc, x);
    glUniform1i(y_tex_loc, y);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void GlFluid2DOpti::diffuse(const int x, const int x0, const float diffusion_rate, const float dt) const {
    glUniform1i(mode_loc, DIFFUSE);
    glUniform1i(x_tex_loc, x);
    glUniform1i(y_tex_loc, x0);
    const float a = dt * diffusion_rate * static_cast<float>(width) * static_cast<float>(height);
    glUniform1f(a_loc, a);
    glUniform1f(dt_loc, dt);
    for(int k = 0; k < sub_step; k ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}

void GlFluid2DOpti::advect(const int x, const int x0, float dt) const {
    glUniform1i(mode_loc, ADVECT);
    glUniform1i(x_tex_loc, x);
    glUniform1i(y_tex_loc, x0);
    glUniform1f(dtw_loc, dt * (float)width);
    glUniform1f(dth_loc, dt * (float)height);
    glUniform1i(width_loc, width);
    glUniform1i(height_loc, height);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void GlFluid2DOpti::project() const {
    glUniform1i(mode_loc, PROJECT);
    glUniform1f(grid_spacing_loc, grid_spacing);
    glUniform1i(stage_loc, 1);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glUniform1i(stage_loc, 2);
    for(int k = 0; k < sub_step; k ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    glUniform1i(stage_loc, 3);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void GlFluid2DOpti::set_bounds_vel() const {
    glUniform1i(mode_loc, BOUND_V);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void GlFluid2DOpti::density_step(float dt) {

}

void GlFluid2DOpti::pressure_step(float dt) {

}

void GlFluid2DOpti::velocity_step(float dt) {
    add_source (U_T, U_PREV_T, dt);
    add_source (V_T, V_PREV_T, dt);
    swap(U_T, U_PREV_T); diffuse (U_T, U_PREV_T, viscosity, dt);
    swap(V_T, V_PREV_T); diffuse (V_T, V_PREV_T, viscosity, dt);
    project();
    set_bounds_vel();
    swap(U_T, U_PREV_T);
    swap(V_T, V_PREV_T);
    advect(U_T, U_PREV_T, dt);
    advect(V_T, V_PREV_T, dt);
    project();
    set_bounds_vel();
}

GLuint GlFluid2DOpti::draw_step(DRAW_MODE mode) {
    glUniform1i(mode_loc, DRAW);
    glUniform1i(draw_mode_loc, mode);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    return color_tex;
}

void GlFluid2DOpti::debug() {

}