//
// Created by Rémi on 15/09/2024.
//

#include "../../../include/sim/2D/GlFluid2DOpti.h"
#include "../../../include/shaders/compute.h"



GlFluid2DOpti::GlFluid2DOpti(
        const int width,
        const int height,
        const int cell_size,
        const float diff,
        const float visc,
        const int sub_step,
        const float add_r,
        const float add_i
)
        : Fluid2D(width * cell_size, height * cell_size, add_r, add_i) {
    // ----------{ init variables }----------

    this->width = width;
    this->height = height;
    this->cell_size = cell_size;
    this->diffusion = diff;
    this->viscosity = visc;
    this->sub_step = sub_step;
    this->grid_spacing = 1.f / static_cast<float>(height);
    const int gridSize = width * height;
    grid = new float[gridSize]();
    dens_permanent = new float[gridSize]();
    u_permanent = new float[gridSize]();
    v_permanent = new float[gridSize]();
    const auto* empty = new float[gridSize]();
    const auto* emptyVec2 = new float[gridSize * 2]();
    const auto* emptyVec4 = new float[gridSize * 4]();
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
    dens_permanent_tex = createTextureVec1(empty, width, height);
    pressure_tex       = createTextureVec1(empty, width, height);
    vel_tex            = createTextureVec2(emptyVec2, width, height);
    vel_prev_tex       = createTextureVec2(emptyVec2, width, height);
    vel_permanent_tex  = createTextureVec2(emptyVec2, width, height);
    color_tex          = createTextureVec4(emptyVec4, width, height);
    // ---------- { Compute programs }----------
    stepsProgram = createComputeProgram("../shaders/computes/steps.glsl");
    glUseProgram(stepsProgram);

    glBindImageTexture(0, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, dens_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, dens_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, dens_permanent_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, pressure_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(5, vel_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture(6, vel_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture(7, vel_permanent_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture(8, color_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

GlFluid2DOpti::~GlFluid2DOpti() {
    delete[] grid;
    delete[] dens_permanent;
    delete[] u_permanent;
    delete[] v_permanent;

    glDeleteTextures(1, &grid_tex);
    glDeleteTextures(1, &dens_tex);
    glDeleteTextures(1, &dens_prev_tex);
    glDeleteTextures(1, &dens_permanent_tex);
    glDeleteTextures(1, &pressure_tex);
    glDeleteTextures(1, &vel_tex);
    glDeleteTextures(1, &vel_permanent_tex);
    glDeleteTextures(1, &vel_prev_tex);
    glDeleteTextures(1, &color_tex);

    glDeleteProgram(stepsProgram);
}

void GlFluid2DOpti::density_step(float dt) {

}

void GlFluid2DOpti::velocity_step(float dt) {

}

void GlFluid2DOpti::pressure_step(float dt) {

}

GLuint GlFluid2DOpti::draw_step(DRAW_MODE mode) {
    return 0;
}

void GlFluid2DOpti::debug() {

}

void GlFluid2DOpti::add_source(TEXTURES x, TEXTURES s, float dt) {

}

void GlFluid2DOpti::swap(TEXTURES x, TEXTURES y) noexcept {

}

void GlFluid2DOpti::diffuse_vel(float dt) {

}

void GlFluid2DOpti::advect_vel(float dt) {

}

void GlFluid2DOpti::project_vel() {

}

void GlFluid2DOpti::set_bounds_vel() {

}

void GlFluid2DOpti::add(const int i,const int j ,const float r ,const float u_intensity,const float v_intensity,const TEXTURES tex,const float dt) {

}

void GlFluid2DOpti::input_step(float r, float intensity, float dt) {
    //glUniform1f(glGetUniformLocation(stepsProgram, "dt"), dt);
    //glUniform1f(glGetUniformLocation(stepsProgram, "r"), r);
    //glUniform1f(glGetUniformLocation(stepsProgram, "intensity"), intensity);
    if (left_mouse_pressed || right_mouse_pressed || middle_mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / cell_size;
        const int j = static_cast<int>((static_cast<float>(cell_size * height) - mouse_y)) / cell_size;

        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            if(left_mouse_pressed || middle_mouse_pressed) {
                if (left_mouse_pressed){
                    //add(i, j, r,  (mouse_x - force_x), u_tex, dt);
                    //add(i, j, r, -(mouse_y - force_y), v_tex, dt);
                    add(i, j, r, (mouse_x - force_x),  -(mouse_y - force_y), VEL_T, dt);

                }
                if(middle_mouse_pressed) {
                    //add(i, j, r, intensity, u_permanent_tex, dt);
                    //add(i, j, r, 0, v_permanent_tex, dt);
                    add(i, j, r, intensity, 0, VEL_PERM_T, dt);
                }
                if(right_mouse_pressed) {
                    //add(i, j, r, -intensity, u_permanent_tex, dt);
                    //add(i, j, r, 0, v_permanent_tex, dt);
                    add(i, j, r, -intensity,  0, VEL_PERM_T, dt);

                }
            }
        }
        force_x = mouse_x;
        force_y = mouse_y;
    }

    add_source(DENS_T, DENS_PERM_T, dt);
    add_source(VEL_T, VEL_PERM_T, dt);

}

