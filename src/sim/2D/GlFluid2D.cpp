//
// Created by remi.cazoulat on 20/08/2024.
//

#include "../../../include/sim/2D/GlFluid2D.h"
#include "../../../include/shaders/compute.h"

GlFluid2D::GlFluid2D(GLFWwindow* window, SimData* simData)
        : Fluid2D(window, simData)
{
    // init variables

    // init debug variables
    this->BINDING_TIME = 0;
    this->DISPATCH_TIME = 0;

    this->INPUT_STEP_TIME = 0;
    this->DENSITY_STEP_TIME = 0;
    this->VELOCITY_STEP_TIME = 0;
    this->PRESSURE_STEP_TIME = 0;
    this->DRAW_STEP_TIME = 0;
    this->TOTAL_STEPS = 0;
    // init arrays
    int grid_size = width * height;
    grid = new float[grid_size]();
    const auto* empty = new float[grid_size]();
    const auto* emptyVec4 = new float[grid_size * 4]();
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
    dens_perm_tex = createTextureVec1(empty, width, height);
    pressure_tex       = createTextureVec1(empty, width, height);
    u_tex              = createTextureVec1(empty, width, height);
    v_tex              = createTextureVec1(empty, width, height);
    u_perm_tex    = createTextureVec1(empty, width, height);
    v_perm_tex    = createTextureVec1(empty, width, height);
    u_prev_tex         = createTextureVec1(empty, width, height);
    v_prev_tex         = createTextureVec1(empty, width, height);
    color_tex          = createTextureVec4(emptyVec4, width, height);
    // ---------- { Compute programs }----------
    inputProgram   = createComputeProgram("../shaders/computes/add_input.glsl");
    addProgram     = createComputeProgram("../shaders/computes/add_source.glsl");
    diffuseProgram = createComputeProgram("../shaders/computes/diffuse.glsl");
    advectProgram  = createComputeProgram("../shaders/computes/advect.glsl");
    projectProgram = createComputeProgram("../shaders/computes/project.glsl");
    boundProgram   = createComputeProgram("../shaders/computes/set_vel_bound.glsl");
    drawProgram    = createComputeProgram("../shaders/computes/draw.glsl");

    delete[] empty;
    delete[] emptyVec4;
}

GlFluid2D::~GlFluid2D() {
    delete[] grid;
    glDeleteTextures(1, &grid_tex);
    glDeleteTextures(1, &dens_tex);
    glDeleteTextures(1, &dens_prev_tex);
    glDeleteTextures(1, &dens_perm_tex);
    glDeleteTextures(1, &pressure_tex);
    glDeleteTextures(1, &u_tex);
    glDeleteTextures(1, &v_tex);
    glDeleteTextures(1, &u_perm_tex);
    glDeleteTextures(1, &v_perm_tex);
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
void GlFluid2D::add_source(const GLuint x, const GLuint s, const float dt) {
    glUseProgram(addProgram);
    glUniform1f(glGetUniformLocation(addProgram, "dt"), dt);
    bind_and_run({x, s}, 1);
}

void GlFluid2D::swap(GLuint &x, GLuint &y) noexcept {
    const GLuint tmp = x;
    x = y;
    y = tmp;
}

void GlFluid2D::diffuse(const GLuint x, const GLuint x0, const float diff, const float dt) {
    const float a = dt * diff * static_cast<float>(width) * static_cast<float>(height);
    glUseProgram(diffuseProgram);
    glUniform1f(glGetUniformLocation(diffuseProgram, "a"), a);
    bind_and_run({x, x0, grid_tex},  simData->sub_step);
}

void GlFluid2D::advect(const GLuint z, const GLuint z0, const float dt) {
    const float dtw = dt * static_cast<float>(width);
    const float dth = dt * static_cast<float>(height);
    glUseProgram(advectProgram);
    glUniform1f(glGetUniformLocation(advectProgram, "dtw"), dtw);
    glUniform1f(glGetUniformLocation(advectProgram, "dth"), dth);
    glUniform1i(glGetUniformLocation(advectProgram, "width"), width);
    glUniform1i(glGetUniformLocation(advectProgram, "height"), height);
    bind_and_run({z, z0, u_prev_tex, v_prev_tex, grid_tex}, 1);
}

void GlFluid2D::project() {
    auto previous_time = glfwGetTime();
    glUseProgram(projectProgram);
    glUniform1f(glGetUniformLocation(projectProgram, "h_w"),  simData->h_w);
    glUniform1f(glGetUniformLocation(projectProgram, "h_h"),  simData->h_h);
    const GLint step_loc = glGetUniformLocation(projectProgram, "step");
    bind({u_tex, v_tex, u_prev_tex, v_prev_tex, grid_tex});

    auto current_time = glfwGetTime();
    BINDING_TIME += current_time - previous_time;
    // step 1
    previous_time = glfwGetTime();
    glUniform1i(step_loc, 1);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    // step 2
    glUniform1i(step_loc, 2);
    for(int k = 0; k <  simData->sub_step; k ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    // step 3
    glUniform1i(step_loc, 3);
    glDispatchCompute(width / 64,height / 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    // end
    current_time = glfwGetTime();
    DISPATCH_TIME += current_time - previous_time;
}

void GlFluid2D::set_vel_bound() {
    glUseProgram(boundProgram);
    bind_and_run({u_tex, v_tex, grid_tex}, 1);
}

void GlFluid2D::density_step(float dt) {
    const auto step_time = glfwGetTime();

    //TODO: add_input density step

    const auto end_step_time = glfwGetTime();
    DENSITY_STEP_TIME += end_step_time - step_time;
}

void GlFluid2D::velocity_step(const float dt) {
    const auto step_time = glfwGetTime();
    TOTAL_STEPS += 1;

    add_source (u_tex, u_prev_tex, dt);
    add_source (v_tex, v_prev_tex, dt);
    swap(u_prev_tex, u_tex); diffuse (u_tex, u_prev_tex,  simData->viscosity, dt);
    swap(v_prev_tex, v_tex); diffuse (v_tex, v_prev_tex,  simData->viscosity, dt);
    project ();
    set_vel_bound();
    swap(u_prev_tex, u_tex);
    swap(v_prev_tex, v_tex);
    advect (u_tex, u_prev_tex, dt);
    advect (v_tex, v_prev_tex, dt);
    project ();
    set_vel_bound();

    const auto end_step_time = glfwGetTime();
    VELOCITY_STEP_TIME += end_step_time - step_time;
}

void GlFluid2D::pressure_step(float dt) {
    const auto step_time = glfwGetTime();

    //TODO : add_input pressure step

    const auto end_step_time = glfwGetTime();
    PRESSURE_STEP_TIME += end_step_time - step_time;
}

GLuint GlFluid2D::draw_step(const DRAW_MODE mode) {
    const auto step_time = glfwGetTime();

    glUseProgram(drawProgram);
    glUniform1i(glGetUniformLocation(drawProgram, "draw_mode"), mode);
    glBindImageTexture(2, color_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    bind_and_run({u_tex, v_tex}, 1);
    const auto end_step_time = glfwGetTime();
    DRAW_STEP_TIME += end_step_time - step_time;

    renderer->rendering(color_tex);
    return color_tex;
}

void GlFluid2D::add_input(const int i, const int j, const float r, const float intensity, const GLuint tex, const float dt) {
    glUseProgram(inputProgram);
    glUniform1i(glGetUniformLocation(inputProgram, "i"), i);
    glUniform1i(glGetUniformLocation(inputProgram, "j"), j);
    glUniform1f(glGetUniformLocation(inputProgram, "r"), r);
    glUniform1f(glGetUniformLocation(inputProgram, "intensity"), intensity);
    glUniform1f(glGetUniformLocation(inputProgram, "dt"), dt);
    bind_and_run({tex, grid_tex}, 1);
}

void GlFluid2D::input_step( const float dt) {
    const auto step_time = glfwGetTime();

    if (left_mouse_pressed || right_mouse_pressed || middle_mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / simData->cell_size;
        const int j = static_cast<int>((static_cast<float>(simData->cell_size * height) - mouse_y)) / simData->cell_size;
        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            if (left_mouse_pressed){
                if(simData->smoke) {
                    if(simData->smoke_add) {
                        add_input(i, j,simData->smoke_radius,simData->smoke_intensity, dens_tex, dt);
                    }
                    if(simData->smoke_perm) {
                        add_input(i, j, simData->smoke_radius, simData->smoke_intensity, dens_perm_tex, dt);
                    }
                    if(simData->smoke_remove) {
                        //TODO: make smoke_remove
                    }
                }
                if(simData->velocity) {
                    if(simData->vel_add) {
                        add_input(i, j,simData->vel_radius, (mouse_x - force_x), u_tex, dt);
                        add_input(i, j,simData->vel_radius,-(mouse_y - force_y), v_tex, dt);
                        printf("passing here \n");
                    }
                    if(simData->vel_perm) {
                        add_input(i, j,simData->vel_radius, simData->vel_intensity[0], u_tex, dt);
                        add_input(i, j,simData->vel_radius,simData->vel_intensity[1], v_tex, dt);
                    }
                    if(simData->vel_remove) {
                        //TODO: make vel_remove
                    }
                }
                if(simData->obstacles) {
                    if(simData->obstacles_add) {
                        //TODO: make obstacles_add
                    }
                    if(simData->obstacles_remove) {
                        //TODO: make obstacles_remove
                    }
                }
            }
        }
        force_x = mouse_x;
        force_y = mouse_y;
    }
    add_source(dens_tex, dens_perm_tex, dt);
    add_source(u_tex, u_perm_tex, dt);
    add_source(v_tex, v_perm_tex, dt);

    const auto end_step_time = glfwGetTime();
    INPUT_STEP_TIME += end_step_time - step_time;
}

void GlFluid2D::bind_and_run(const std::vector<GLuint> &textures, const int how_many_time) {
    auto start_time = glfwGetTime();
    for (size_t i = 0; i < textures.size(); i++) {
        glBindImageTexture(i, textures[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    }
    auto bind_time = glfwGetTime();
    BINDING_TIME += bind_time - start_time;
    for(int i = 0; i < how_many_time; i ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    auto dispatch_time = glfwGetTime();
    DISPATCH_TIME += dispatch_time - bind_time;
}

void GlFluid2D::bind(const std::vector<GLuint> &textures) {
    for (size_t i = 0; i < textures.size(); i++) {
        glBindImageTexture(i, textures[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    }
}

void GlFluid2D::debug() {
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
    printf("=========[ GlFluid2D Debug ]=========\n");
    printf("Input time: %f ms (%.2f %%)\n", INPUT_STEP_TIME * 1000, percent_input_time);
    printf("Density time: %f ms (%.2f %%)\n", DENSITY_STEP_TIME * 1000, percent_density_time);
    printf("Velocity time: %f ms (%.2f %%)\n", VELOCITY_STEP_TIME * 1000, percent_velocity_time);
    printf("Pressure time: %f ms (%.2f %%)\n", PRESSURE_STEP_TIME * 1000, percent_pressure_time);
    printf("Draw time: %f ms (%.2f %%)\n", DRAW_STEP_TIME * 1000, percent_draw_time);
    printf("\n");
    BINDING_TIME /= TOTAL_STEPS;
    DISPATCH_TIME /= TOTAL_STEPS;

    total = BINDING_TIME + DISPATCH_TIME;
    const auto percent_binding_time = BINDING_TIME / total * 100;
    const auto percent_dispatch_time = DISPATCH_TIME / total * 100;
    printf("Binding time: %f ms (%.2f %%)\n", BINDING_TIME * 1000, percent_binding_time);
    printf("Dispatch time: %f ms (%.2f %%)\n", DISPATCH_TIME * 1000, percent_dispatch_time);
}

