//
// Created by Rémi on 15/09/2024.
//

#include "../../../include/sim/2D/GlFluid2DOpti.h"
#include "../../../include/shaders/compute.h"

enum COMPUTE_MODE {
    INPUT,     // 0
    SOURCE,    // 1
    SWAP,      // 2
    DIFFUSE,   // 3
    ADVECT,    // 4
    PROJECT,   // 5
    BOUND_D,   // 6
    BOUND_V    // 7
};

enum TEXTURES {
    GRID_T,      // 0
    DENS_T,      // 1
    DENS_PREV_T, // 2
    DENS_PERM_T, // 3
    U_T,         // 4
    V_T,         // 5
    U_PREV_T,    // 6
    V_PREV_T,    // 7
    U_PERM_T,    // 8
    V_PERM_T,    // 9
    COLOR_T      // 10
};

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
    dens_perm_tex      = createTextureVec1(empty, width, height);
    pressure_tex       = createTextureVec1(empty, width, height);
    u_tex           = createTextureVec1(empty, width, height);
    u_prev_tex      = createTextureVec1(empty, width, height);
    u_perm_tex      = createTextureVec1(empty, width, height);
    v_tex           = createTextureVec1(empty, width, height);
    v_prev_tex      = createTextureVec1(empty, width, height);
    v_perm_tex      = createTextureVec1(empty, width, height);
    color_tex          = createTextureVec4(emptyVec4, width, height);
    // ---------- { Compute programs }----------
    stepsProgram = createComputeProgram("../shaders/computes/steps.glsl");
    glUseProgram(stepsProgram);

    glBindImageTexture(0, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, dens_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, dens_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, dens_perm_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, pressure_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, u_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, u_perm_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, v_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, v_perm_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
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
    glDeleteTextures(1, &dens_perm_tex);
    glDeleteTextures(1, &pressure_tex);
    glDeleteTextures(1, &u_tex);
    glDeleteTextures(1, &u_prev_tex);
    glDeleteTextures(1, &u_perm_tex);
    glDeleteTextures(1, &v_tex);
    glDeleteTextures(1, &v_prev_tex);
    glDeleteTextures(1, &v_perm_tex);
    glDeleteTextures(1, &color_tex);

    glDeleteProgram(stepsProgram);
}

void GlFluid2DOpti::density_step(float dt) {

}

void GlFluid2DOpti::pressure_step(float dt) {

}

GLuint GlFluid2DOpti::draw_step(DRAW_MODE mode) {
    return 0;
}

void GlFluid2DOpti::debug() {

}

void GlFluid2DOpti::add_source(const int x, const int s, float dt) {

}

void GlFluid2DOpti::swap(const int x_tex, const int y_tex) {

}

void GlFluid2DOpti::diffuse(const int x_tex, const int x0_tex, const float diffusion_rate, const float dt) {

}

void GlFluid2DOpti::advect(const int x_tex, const int x0_tex, float dt) {

}

void GlFluid2DOpti::project() {

}

void GlFluid2DOpti::set_bounds_vel() {

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

void GlFluid2DOpti::add(const int i,const int j ,const float r ,const float intensity, const int tex, const float dt) {

}

void GlFluid2DOpti::input_step(float r, float* intensities, float dt) {
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

