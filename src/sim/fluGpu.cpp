//
// Created by remi.cazoulat on 20/08/2024.
//

#include "../../include/sim/fluGpu.h"




fluGpu::fluGpu(const int width, const int height, const int cell_size) {
    /*
    this->width = width;
    this->height = height;
    this->cell_size = cell_size;
    const int gridSize = width * height;
    color = new GLfloat[gridSize * 3]();

    constexpr auto circleCoord = glm::vec2(128 / 2, 72 / 8);
    for(int j = 0; j < height ; j ++) {
        const int jg = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = i + jg;
            if (i == 0 || i == width - 1 || j == 0 || j == height - 1) {
                grid[index] = 0.0;
            }
            else {
                grid[index] = 1.0;
            }
            constexpr float radius = 20.0;
            const float distance = glm::distance(glm::vec2(i, j), circleCoord);
            pressure[index] = distance < radius ? 1.0 : 0.2;

            if( j > height / 2 - 10 && j < height / 2 + 10) {
                velocity[index * 2] = 1.0;
                velocity[index * 2 + 1] = 0.0;
            } else {
                velocity[index * 2] = 0.0;
                velocity[index * 2 + 1] = 0.0;
            }
        }
    }

    // ---------- { Init Textures }----------
    velocityTex = createTextureVec2(velocity, width, height);
    const GLuint gridTex = createTextureVec1(grid, width, height);
    const GLuint resultsTex = createTextureVec1(results, width, height);
    pressureTex = createTextureVec1(pressure, width, height);

    // ---------- { Compute program }----------
    projectProgram = createComputeProgram("../shaders/computes/project.glsl");
    swapProgram = createComputeProgram("../shaders/computes/swap.glsl");

    glUseProgram(projectProgram);
    glBindImageTexture (0, velocityTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, resultsTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glUseProgram(swapProgram);
    glBindImageTexture (0, velocityTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, resultsTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    */
}

fluGpu::~fluGpu() {
    /*
    delete[] velocity;
    delete[] grid;
    delete[] results;
    delete[] pressure;
        */
}
///////////////////////////
/// maths methods
///////////////////////////
void fluGpu::add_source(GLuint x, const GLuint s, float dt) const {

}

void fluGpu::diffuse(int b, GLuint x, const GLuint x0, float diff, float dt) const {

}

void fluGpu::advect(int b, GLuint z, const GLuint z0, const GLuint u_vel, const GLuint v_vel, float dt) const {

}

void fluGpu::project(GLuint p, GLuint div) const {
    /*
    for(int i = 0; i < 1; i ++) {
        glUseProgram(projectProgram);
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glUseProgram(swapProgram);
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
     */
}

void fluGpu::inputs_step(int r, float intensity) const {

}

void fluGpu::density_step(float dt) {

}

void fluGpu::velocity_step(float dt) {

}

void fluGpu::calculate_pressure(float dt) const {

}

GLuint fluGpu::draw(DRAW_MODE mode) const {
    return colorTex;
}
///////////////////////
/// other methods
///////////////////////
void fluGpu::add_dens(int x, int y) const {

}

void fluGpu::add_vel(int x, int y, float u_intensity, float v_intensity) const {

}

void fluGpu::add_permanent_dens(int x, int y, float radius) const {

}

void fluGpu::add_permanent_vel(int x, int y, float u_intensity, float v_intensity) const {

}

void fluGpu::add_all_permanent_step() const {

}









