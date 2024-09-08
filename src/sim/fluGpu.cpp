//
// Created by remi.cazoulat on 20/08/2024.
//

#include "../../include/sim/fluGpu.h"

static float force_x = 0.0f, force_y = 0.0f, mouse_x = 0.0f, mouse_y = 0.0f;
static int left_mouse_pressed = 0, right_mouse_pressed = 0, middle_mouse_pressed = 0;

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
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

    dens_prev = new float[gridSize]();
    u_prev = new float[gridSize]();
    v_prev = new float[gridSize]();

    dens_permanent = new float[gridSize]();
    u_permanent = new float[gridSize]();
    v_permanent = new float[gridSize]();

    auto* empty = new float[gridSize]();
    auto* emptyVec3 = new float[gridSize * 3]();
    // setting initial bounds and obstacles
    const float r = 10.f;
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
    gridTex           = createTextureVec1(grid, width, height);
    densTex           = createTextureVec1(empty, width, height);
    dens_prevTex      = createTextureVec1(empty, width, height);
    dens_permanentTex = createTextureVec1(empty, width, height);
    pressureTex       = createTextureVec1(empty, width, height);
    uTex              = createTextureVec1(empty, width, height);
    vTex              = createTextureVec1(empty, width, height);
    u_permanentTex    = createTextureVec1(empty, width, height);
    v_permanentTex    = createTextureVec1(empty, width, height);
    u_prevTex         = createTextureVec1(empty, width, height);
    v_prevTex         = createTextureVec1(empty, width, height);
    colorTex          = createTextureVec3(emptyVec3, width, height);

    // ---------- { Compute programs }----------
    addProgram     = createComputeProgram("../shaders/computes/add_source.glsl");
    advectProgram  = createComputeProgram("../shaders/computes/advect.glsl");
    diffuseProgram = createComputeProgram("../shaders/computes/diffuse.glsl");
    projectProgram = createComputeProgram("../shaders/computes/project.glsl");
    boundProgram   = createComputeProgram("../shaders/computes/set_vel_bound.glsl");
    swapProgram    = createComputeProgram("../shaders/computes/swap.glsl");
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
    delete[] emptyVec3;
}

fluGpu::~fluGpu() {
    delete[] grid;
    delete[] dens_prev;
    delete[] u_prev;
    delete[] v_prev;
    delete[] dens_permanent;
    delete[] u_permanent;
    delete[] v_permanent;
}
// ////////////////////////
//  maths methods
// ////////////////////////
void fluGpu::add_source(GLuint x, const GLuint s, float dt) const {
    glUseProgram(addProgram);
    glBindImageTexture(0, x, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, s, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(addProgram, "dt"), dt);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void fluGpu::swap(GLuint x, GLuint y) const {
    glUseProgram(swapProgram);
    glBindImageTexture(0, x, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, y, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void fluGpu::diffuse(GLuint x, const GLuint x0, float diff, float dt) const {
    const float a = dt * diff * static_cast<float>(width) * static_cast<float>(height);
    glUseProgram(diffuseProgram);
    glBindImageTexture(0, x, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, x0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(addProgram, "a"), a);
    for(int k = 0; k < sub_step; k ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

}

void fluGpu::advect(GLuint z, const GLuint z0, const GLuint u_vel, const GLuint v_vel, float dt) const {
    const float dtw = dt * static_cast<float>(width);
    const float dth = dt * static_cast<float>(height);
    glUseProgram(advectProgram);
    glBindImageTexture(0, z, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, z0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, u_vel, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, v_vel, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(addProgram, "dtw"), dtw);
    glUniform1f(glGetUniformLocation(addProgram, "dth"), dth);
    glUniform1i(glGetUniformLocation(addProgram, "width"), width);
    glUniform1i(glGetUniformLocation(addProgram, "height"), height);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void fluGpu::project(GLuint p, GLuint div) const {
    glUseProgram(projectProgram);
    glBindImageTexture(0, uTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, vTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, p, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, div, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(addProgram, "h"), grid_spacing);
    GLint step_loc = glGetUniformLocation(addProgram, "step");

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

}

void fluGpu::density_step(float dt) {

}

void fluGpu::velocity_step(float dt) {
    add_source (uTex, u_prevTex, dt);
    add_source (vTex, v_prevTex, dt);
    swap(u_prevTex, uTex); diffuse (uTex, u_prevTex, viscosity, dt);
    swap(v_prevTex, vTex); diffuse (vTex, v_prevTex, viscosity, dt);
    project (u_prevTex, v_prevTex);
    swap(u_prevTex, uTex);
    swap(v_prevTex, vTex);
    advect (uTex, u_prevTex, u_prevTex, v_prevTex, dt);
    advect (vTex, v_prevTex, u_prevTex, v_prevTex, dt);
    set_vel_bound();
    project (u_prevTex, v_prevTex);
}

void fluGpu::calculate_pressure(float dt) const {

}

GLuint fluGpu::draw(DRAW_MODE mode) const {
    return colorTex;
}


void fluGpu::add(const int x,const int y, float *t,const float intensity) const {

}

void fluGpu::input_step(const int r, const float intensity, const float dt) {
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

                                }
                                if (left_mouse_pressed){
                                    //add_vel(i + x, j + y, (mouse_x - force_x) , (mouse_y - force_y));
                                    add(i + x,j + y, u_prev, (mouse_x - force_x));
                                    add(i + x,j + y, v_prev, -(mouse_y - force_y));

                                }
                                if(right_mouse_pressed) {
                                    add(i + x, j + y, dens_prev, intensity);
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
    dens_permanentTex = createTextureVec1(dens_permanent, width, height);
    u_permanentTex    = createTextureVec1(u_permanent, width, height);
    v_permanentTex    = createTextureVec1(v_permanent, width, height);
    add_source(densTex, dens_permanentTex, dt);
    add_source(uTex, u_permanentTex, dt);
    add_source(vTex, v_permanentTex, dt);
}











