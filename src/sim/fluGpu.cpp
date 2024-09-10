//
// Created by remi.cazoulat on 20/08/2024.
//

#include "../../include/sim/fluGpu.h"

static float force_x = 0.0f, force_y = 0.0f, mouse_x = 0.0f, mouse_y = 0.0f;
static int left_mouse_pressed = 0, right_mouse_pressed = 0, middle_mouse_pressed = 0;

static void mouse_button_callback(GLFWwindow* window, const int button, const int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        left_mouse_pressed = 1;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        left_mouse_pressed = 0;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        right_mouse_pressed = 1;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        right_mouse_pressed = 0;
    }
    if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        middle_mouse_pressed = 1;
    } else if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
        middle_mouse_pressed = 0;
    }
}

static void cursor_position_callback(GLFWwindow* window, const double xpos, const double ypos) {
    mouse_x = static_cast<float>(xpos);
    mouse_y = static_cast<float>(ypos);
}

fluGpu::fluGpu(GLFWwindow* window, const int width, const int height, const int cell_size, const float diff, const float visc, const int sub_step) {
    this->window = window;
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
    const auto* emptyVec4 = new float[gridSize * 4]();
    // setting initial bounds and obstacles
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

    // ---------- { Init Textures }----------
    grid_tex           = createTextureVec1(grid, width, height);
    dens_tex           = createTextureVec1(empty, width, height);
    dens_prev_tex      = createTextureVec1(empty, width, height);
    dens_permanent_tex = createTextureVec1(empty, width, height);
    pressure_tex       = createTextureVec1(empty, width, height);
    u_tex              = createTextureVec1(empty, width, height);
    v_tex              = createTextureVec1(empty, width, height);
    u_permanent_tex    = createTextureVec1(empty, width, height);
    v_permanent_tex    = createTextureVec1(empty, width, height);
    u_prev_tex         = createTextureVec1(empty, width, height);
    v_prev_tex         = createTextureVec1(empty, width, height);
    color_tex          = createTextureVec4(emptyVec4, width, height);

    // ---------- { Compute programs }----------
    addProgram     = createComputeProgram("../shaders/computes/add_source.glsl");
    advectProgram  = createComputeProgram("../shaders/computes/advect.glsl");
    diffuseProgram = createComputeProgram("../shaders/computes/diffuse.glsl");
    projectProgram = createComputeProgram("../shaders/computes/project.glsl");
    boundProgram   = createComputeProgram("../shaders/computes/set_vel_bound.glsl");
    swapProgram    = createComputeProgram("../shaders/computes/swap.glsl");
    drawProgram    = createComputeProgram("../shaders/computes/draw.glsl");
    /*
    glUseProgram(projectProgram);
    glBindImageTexture (0, velocityTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, resultsTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glUseProgram(swapProgram);
    glBindImageTexture (0, velocityTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, resultsTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    */
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    delete[] empty;
    delete[] emptyVec4;
}

fluGpu::~fluGpu() {
    delete[] grid;
    delete[] dens_permanent;
    delete[] u_permanent;
    delete[] v_permanent;
}
// ////////////////////////
//  maths methods
// ////////////////////////
void fluGpu::add_source(const GLuint x, const GLuint s,const float dt) const {
    glUseProgram(addProgram);
    glBindImageTexture(0, x, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, s, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(addProgram, "dt"), dt);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void fluGpu::swap( GLuint &x,  GLuint &y) noexcept {
    /*
    glUseProgram(swapProgram);
    glBindImageTexture(0, x, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, y, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    */
    const GLuint tmp = x;
    x = y;
    y = tmp;
}

void fluGpu::diffuse(const GLuint x, const GLuint x0,const float diff,const float dt) const {
    const float a = dt * diff * static_cast<float>(width) * static_cast<float>(height);
    glUseProgram(diffuseProgram);
    glBindImageTexture(0, x, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, x0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(diffuseProgram, "a"), a);
    for(int k = 0; k < sub_step; k ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

}

void fluGpu::advect(const GLuint z, const GLuint z0, const GLuint u_vel, const GLuint v_vel, const float dt) const {
    const float dtw = dt * static_cast<float>(width);
    const float dth = dt * static_cast<float>(height);
    glUseProgram(advectProgram);
    glBindImageTexture(0, z, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, z0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, u_vel, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, v_vel, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(advectProgram, "dtw"), dtw);
    glUniform1f(glGetUniformLocation(advectProgram, "dth"), dth);
    glUniform1i(glGetUniformLocation(advectProgram, "width"), width);
    glUniform1i(glGetUniformLocation(advectProgram, "height"), height);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void fluGpu::project(const GLuint p, const GLuint div) const {
    glUseProgram(projectProgram);
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, p, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, div, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(projectProgram, "h"), grid_spacing);
    const GLint step_loc = glGetUniformLocation(projectProgram, "step");

    // step 1
    glUniform1i(step_loc, 1);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // step 2
    glUniform1i(step_loc, 2);
    for(int k = 0; k < sub_step; k ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    // step 3
    glUniform1i(step_loc, 3);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // unbind
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void fluGpu::set_vel_bound() const {
    glUseProgram(boundProgram);
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void fluGpu::density_step(float dt) {

}

void fluGpu::velocity_step(const float dt) {
    add_source (u_tex, u_prev_tex, dt);
    add_source (v_tex, v_prev_tex, dt);
    swap(u_prev_tex, u_tex); diffuse (u_tex, u_prev_tex, viscosity, dt);
    swap(v_prev_tex, v_tex); diffuse (v_tex, v_prev_tex, viscosity, dt);
    project (u_prev_tex, v_prev_tex);
    set_vel_bound();
    swap(u_prev_tex, u_tex);
    swap(v_prev_tex, v_tex);
    advect (u_tex, u_prev_tex, u_prev_tex, v_prev_tex, dt);
    advect (v_tex, v_prev_tex, u_prev_tex, v_prev_tex, dt);
    set_vel_bound();
    project (u_prev_tex, v_prev_tex);
    set_vel_bound();
}

void fluGpu::calculate_pressure(float dt) const {

}

GLuint fluGpu::draw(const DRAW_MODE mode) const {
    glUseProgram(drawProgram);
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, color_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glUniform1i(glGetUniformLocation(drawProgram, "draw_mode"), mode);

    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    return color_tex;
}


void fluGpu::add(const int x,const int y, float *t,const float intensity) const {
    if(grid[x + y * width] == 0.f) return;
    t[x + y * width] += intensity;
}

void fluGpu::input_step(const int r, const float intensity, const float dt) {
    bool new_permanent = false;
    bool new_temp = false;
    auto* u_temp = new float[width * height]();
    auto* v_temp = new float[width * height]();
    auto* dens_temp = new float[width * height]();
    if (left_mouse_pressed || right_mouse_pressed || middle_mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / cell_size;
        const int j = static_cast<int>((static_cast<float>(cell_size * height) - mouse_y)) / cell_size;

        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            if(left_mouse_pressed || middle_mouse_pressed) {
                for(int x = -r; x <= r; x++) {
                    for(int y = -r; y <= r; y++) {
                        if (i + x >= 1 && i + x < width - 1 && j + y >= 1 && j + y < height - 1) {
                            if (std::sqrt(static_cast<float>(x * x + y * y)) < static_cast<float>(r)) {
                                if(middle_mouse_pressed) {
                                    add(i + x, j + y, u_permanent, 0);
                                    add(i + x, j + y, v_permanent, intensity);
                                    new_permanent = true;
                                }
                                if (left_mouse_pressed){
                                    //add_vel(i + x, j + y, (mouse_x - force_x) , (mouse_y - force_y));
                                    add(i + x,j + y, u_temp, (mouse_x - force_x));
                                    add(i + x,j + y, v_temp, -(mouse_y - force_y));
                                    new_temp = true;
                                }
                                if(right_mouse_pressed) {
                                    add(i + x, j + y, dens_temp, intensity);
                                    new_temp = true;
                                }
                            }
                        }
                    }
                }
                force_x = mouse_x;
                force_y = mouse_y;
            }
        }
    }
    if(new_permanent) {
        dens_permanent_tex = createTextureVec1(dens_permanent, width, height);
        u_permanent_tex    = createTextureVec1(u_permanent, width, height);
        v_permanent_tex    = createTextureVec1(v_permanent, width, height);
    }
    add_source(dens_tex, dens_permanent_tex, dt);
    add_source(u_tex, u_permanent_tex, dt);
    add_source(v_tex, v_permanent_tex, dt);

    if(new_temp) {
        const GLuint densTex_tmp = createTextureVec1(dens_temp, width, height);
        const GLuint uTex_tmp = createTextureVec1(u_temp, width, height);
        const GLuint vTex_tmp = createTextureVec1(v_temp, width, height);
        add_source(dens_tex, densTex_tmp, 1);
        add_source(u_tex, uTex_tmp, 1);
        add_source(v_tex, vTex_tmp, 1);
    }
    delete[] u_temp;
    delete[] v_temp;
    delete[] dens_temp;
}











