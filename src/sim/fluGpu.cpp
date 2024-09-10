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
    // init variables
    this->window = window;
    this->width = width;
    this->height = height;
    this->cell_size = cell_size;
    this->diffusion = diff;
    this->viscosity = visc;
    this->sub_step = sub_step;
    this->grid_spacing = 1.f / static_cast<float>(height);
    // init debug variables
    this->BINDING_TIME = 0;
    this->DISPATCH_TIME = 0;
    this->UNBINDING_TIME = 0;
    this->INPUT_STEP_TIME = 0;
    this->DENSITY_STEP_TIME = 0;
    this->VELOCITY_STEP_TIME = 0;
    this->PRESSURE_STEP_TIME = 0;
    this->DRAW_STEP_TIME = 0;
    this->TOTAL_STEPS = 0;
    // init arrays
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
    inputProgram     = createComputeProgram("../shaders/computes/add_input.glsl");
    addProgram     = createComputeProgram("../shaders/computes/add_source.glsl");
    diffuseProgram = createComputeProgram("../shaders/computes/diffuse.glsl");
    advectProgram  = createComputeProgram("../shaders/computes/advect.glsl");
    projectProgram = createComputeProgram("../shaders/computes/project.glsl");
    boundProgram   = createComputeProgram("../shaders/computes/set_vel_bound.glsl");
    drawProgram    = createComputeProgram("../shaders/computes/draw.glsl");

    glUseProgram(diffuseProgram);
    glBindImageTexture(2, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glUseProgram(advectProgram);
    glBindImageTexture(2, u_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, v_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1i(glGetUniformLocation(advectProgram, "width"), width);
    glUniform1i(glGetUniformLocation(advectProgram, "height"), height);

    glUseProgram(projectProgram);
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, u_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, v_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(projectProgram, "h"), grid_spacing);

    glUseProgram(boundProgram);
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glUseProgram(drawProgram);
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, color_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    // input settings
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

    glDeleteTextures(1, &grid_tex);
    glDeleteTextures(1, &dens_tex);
    glDeleteTextures(1, &dens_prev_tex);
    glDeleteTextures(1, &dens_permanent_tex);
    glDeleteTextures(1, &pressure_tex);
    glDeleteTextures(1, &u_tex);
    glDeleteTextures(1, &v_tex);
    glDeleteTextures(1, &u_permanent_tex);
    glDeleteTextures(1, &v_permanent_tex);
    glDeleteTextures(1, &u_prev_tex);
    glDeleteTextures(1, &v_prev_tex);
    glDeleteTextures(1, &color_tex);

    glDeleteProgram(addProgram);
    glDeleteProgram(advectProgram);
    glDeleteProgram(diffuseProgram);
    glDeleteProgram(projectProgram);
    glDeleteProgram(boundProgram);
    glDeleteProgram(drawProgram);
}
// ////////////////////////
//  maths methods
// ////////////////////////
void fluGpu::add_source(const GLuint x, const GLuint s,const float dt) {
    glUseProgram(addProgram);
    auto previousTime = glfwGetTime();
    glBindImageTexture(0, x, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, s, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(addProgram, "dt"), dt);
    auto currentTime = glfwGetTime();
    BINDING_TIME += currentTime - previousTime;
    previousTime = glfwGetTime();    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    currentTime = glfwGetTime();
    DISPATCH_TIME += currentTime - previousTime;
    previousTime = glfwGetTime();    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    currentTime = glfwGetTime();
    UNBINDING_TIME += currentTime - previousTime;
}

void fluGpu::swap( GLuint &x,  GLuint &y) noexcept {
    const GLuint tmp = x;
    x = y;
    y = tmp;
}

void fluGpu::diffuse(const GLuint x, const GLuint x0,const float diff,const float dt) {
    const float a = dt * diff * static_cast<float>(width) * static_cast<float>(height);
    glUseProgram(diffuseProgram);
    auto previous_time = glfwGetTime();
    glBindImageTexture(0, x, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, x0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(diffuseProgram, "a"), a);
    auto current_time = glfwGetTime();
    BINDING_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    for(int k = 0; k < sub_step; k ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    current_time = glfwGetTime();
    DISPATCH_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    current_time = glfwGetTime();
    UNBINDING_TIME += current_time - previous_time;

}

void fluGpu::advect(const GLuint z, const GLuint z0, const float dt) {
    const float dtw = dt * static_cast<float>(width);
    const float dth = dt * static_cast<float>(height);
    glUseProgram(advectProgram);
    auto previous_time = glfwGetTime();
    glBindImageTexture(0, z, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, z0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, u_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, v_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(advectProgram, "dtw"), dtw);
    glUniform1f(glGetUniformLocation(advectProgram, "dth"), dth);
    glUniform1i(glGetUniformLocation(advectProgram, "width"), width);
    glUniform1i(glGetUniformLocation(advectProgram, "height"), height);
    auto current_time = glfwGetTime();
    BINDING_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    current_time = glfwGetTime();
    DISPATCH_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    current_time = glfwGetTime();
    UNBINDING_TIME += current_time - previous_time;
}

void fluGpu::project() {
    glUseProgram(projectProgram);
    auto previous_time = glfwGetTime();
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, u_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, v_prev_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUniform1f(glGetUniformLocation(projectProgram, "h"), grid_spacing);
    const GLint step_loc = glGetUniformLocation(projectProgram, "step");
    auto current_time = glfwGetTime();
    BINDING_TIME += current_time - previous_time;
    // step 1

    previous_time = glfwGetTime();
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
    current_time = glfwGetTime();
    DISPATCH_TIME += current_time - previous_time;
    // unbind
    previous_time = glfwGetTime();
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(4, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    current_time = glfwGetTime();
    UNBINDING_TIME += current_time - previous_time;
}

void fluGpu::set_vel_bound() {
    glUseProgram(boundProgram);
    auto previous_time = glfwGetTime();
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    auto current_time = glfwGetTime();
    BINDING_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    current_time = glfwGetTime();
    DISPATCH_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    current_time = glfwGetTime();
    UNBINDING_TIME += current_time - previous_time;
}

void fluGpu::density_step(float dt) {
    const auto step_time = glfwGetTime();

    //TODO: add density step

    const auto end_step_time = glfwGetTime();
    DENSITY_STEP_TIME += end_step_time - step_time;
}

void fluGpu::velocity_step(const float dt) {
    const auto step_time = glfwGetTime();

    TOTAL_STEPS += 1;
    add_source (u_tex, u_prev_tex, dt);
    add_source (v_tex, v_prev_tex, dt);
    swap(u_prev_tex, u_tex); diffuse (u_tex, u_prev_tex, viscosity, dt);
    swap(v_prev_tex, v_tex); diffuse (v_tex, v_prev_tex, viscosity, dt);
    project ();
    set_vel_bound();
    swap(u_prev_tex, u_tex);
    swap(v_prev_tex, v_tex);
    advect (u_tex, u_prev_tex, dt);
    advect (v_tex, v_prev_tex, dt);
    set_vel_bound();
    project ();
    //project (u_prev_tex, v_prev_tex);
    set_vel_bound();

    const auto end_step_time = glfwGetTime();
    VELOCITY_STEP_TIME += end_step_time - step_time;
}

void fluGpu::pressure_step(float dt) {
    const auto step_time = glfwGetTime();

    //TODO : add pressure step

    const auto end_step_time = glfwGetTime();
    PRESSURE_STEP_TIME += end_step_time - step_time;
}

GLuint fluGpu::draw_step(const DRAW_MODE mode) {
    const auto step_time = glfwGetTime();

    glUseProgram(drawProgram);
    auto previous_time = glfwGetTime();
    glBindImageTexture(0, u_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, v_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, color_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glUniform1i(glGetUniformLocation(drawProgram, "draw_mode"), mode);
    auto current_time = glfwGetTime();
    BINDING_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    current_time = glfwGetTime();
    DISPATCH_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    current_time = glfwGetTime();
    UNBINDING_TIME += current_time - previous_time;

    const auto end_step_time = glfwGetTime();
    DRAW_STEP_TIME += end_step_time - step_time;
    return color_tex;
}

void fluGpu::add(const int i,const int j, const float r,const float intensity, const GLuint tex, const float dt) {
    glUseProgram(inputProgram);
    auto previous_time = glfwGetTime();
    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, grid_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glUniform1i(glGetUniformLocation(inputProgram, "i"), i);
    glUniform1i(glGetUniformLocation(inputProgram, "j"), j);
    glUniform1f(glGetUniformLocation(inputProgram, "r"), r);
    glUniform1f(glGetUniformLocation(inputProgram, "intensity"), intensity);
    glUniform1f(glGetUniformLocation(inputProgram, "dt"), dt);
    auto current_time = glfwGetTime();
    BINDING_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    current_time = glfwGetTime();
    DISPATCH_TIME += current_time - previous_time;
    previous_time = glfwGetTime();
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    current_time = glfwGetTime();
    UNBINDING_TIME += current_time - previous_time;
}

void fluGpu::input_step(const float r, const float intensity, const float dt) {
    const auto step_time = glfwGetTime();

    if (left_mouse_pressed || right_mouse_pressed || middle_mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / cell_size;
        const int j = static_cast<int>((static_cast<float>(cell_size * height) - mouse_y)) / cell_size;

        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            if(left_mouse_pressed || middle_mouse_pressed) {

                if(middle_mouse_pressed) {
                    add(i, j, r, 0, u_permanent_tex, dt);
                    add(i, j, r, intensity, v_permanent_tex, dt);
                }
                if (left_mouse_pressed){
                    add(i, j, r,  (mouse_x - force_x), u_tex, dt);
                    add(i, j, r, -(mouse_y - force_y), v_tex, dt);
                }
                if(right_mouse_pressed) {
                }
            }
        }
        force_x = mouse_x;
        force_y = mouse_y;
    }

    add_source(dens_tex, dens_permanent_tex, dt);
    add_source(u_tex, u_permanent_tex, dt);
    add_source(v_tex, v_permanent_tex, dt);

    const auto end_step_time = glfwGetTime();
    INPUT_STEP_TIME += end_step_time - step_time;
}

void fluGpu::debug() {
    INPUT_STEP_TIME /= TOTAL_STEPS;
    DENSITY_STEP_TIME /= TOTAL_STEPS;
    VELOCITY_STEP_TIME /= TOTAL_STEPS;
    PRESSURE_STEP_TIME /= TOTAL_STEPS;
    DRAW_STEP_TIME /= TOTAL_STEPS;
    auto total = INPUT_STEP_TIME + DENSITY_STEP_TIME + VELOCITY_STEP_TIME + PRESSURE_STEP_TIME + DRAW_STEP_TIME;
    const auto percent_input_time = INPUT_STEP_TIME / total * 100;
    const auto percent_density_time = DENSITY_STEP_TIME / total * 100;
    const auto percent_velocity_time = VELOCITY_STEP_TIME / total * 100;
    const auto percent_pressure_time = PRESSURE_STEP_TIME / total * 100;
    const auto percent_draw_time = DRAW_STEP_TIME / total * 100;
    printf("\n");
    printf("=========[ fluGpu Debug ]=========\n");
    printf("Input time: %f ms (%.2f %%)\n", INPUT_STEP_TIME * 1000, percent_input_time);
    printf("Density time: %f ms (%.2f %%)\n", DENSITY_STEP_TIME * 1000, percent_density_time);
    printf("Velocity time: %f ms (%.2f %%)\n", VELOCITY_STEP_TIME * 1000, percent_velocity_time);
    printf("Pressure time: %f ms (%.2f %%)\n", PRESSURE_STEP_TIME * 1000, percent_pressure_time);
    printf("Draw time: %f ms (%.2f %%)\n", DRAW_STEP_TIME * 1000, percent_draw_time);
    printf("\n");
    BINDING_TIME /= TOTAL_STEPS;
    DISPATCH_TIME /= TOTAL_STEPS;
    UNBINDING_TIME /= TOTAL_STEPS;
    total = BINDING_TIME + DISPATCH_TIME + UNBINDING_TIME;
    const auto percent_binding_time = BINDING_TIME / total * 100;
    const auto percent_dispatch_time = DISPATCH_TIME / total * 100;
    const auto percent_unbinding_time = UNBINDING_TIME / total * 100;
    printf("Binding time: %f ms (%.2f %%)\n", BINDING_TIME * 1000, percent_binding_time);
    printf("Dispatch time: %f ms (%.2f %%)\n", DISPATCH_TIME * 1000, percent_dispatch_time);
    printf("Unbinding time: %f ms (%.2f %%)\n", UNBINDING_TIME * 1000, percent_unbinding_time);
}











