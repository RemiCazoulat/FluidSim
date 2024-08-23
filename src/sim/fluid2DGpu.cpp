//
// Created by remi.cazoulat on 20/08/2024.
//

#include "../../include/sim/fluid2DGpu.h"




fluid2DGpu::fluid2DGpu(const int width, const int heigth, const int pixelsPerCell, const float fluidDensity) {
    this->width = width;
    this->heigth = heigth;
    this->pixelsPerCell = pixelsPerCell;
    this->fluidDensity = fluidDensity;
    const int gridSize = width * heigth;
    const int gridSizex2 = gridSize * 2;
    velocity = new GLfloat[gridSizex2]();
    grid = new GLfloat[gridSize]();
    results = new GLfloat[gridSize]();
    pressure = new GLfloat[gridSize]();

    constexpr auto circleCoord = glm::vec2(128 / 2, 72 / 8);
    for(int j = 0; j < heigth ; j ++) {
        const int jg = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = i + jg;
            if (i == 0 || i == width - 1 || j == 0 || j == heigth - 1) {
                grid[index] = 0.0;
            }
            else {
                grid[index] = 1.0;
            }
            constexpr float radius = 20.0;
            const float distance = glm::distance(glm::vec2(i, j), circleCoord);
            pressure[index] = distance < radius ? 1.0 : 0.2;

            if( j > heigth / 2 - 10 && j < heigth / 2 + 10) {
                velocity[index * 2] = 1.0;
                velocity[index * 2 + 1] = 0.0;
            } else {
                velocity[index * 2] = 0.0;
                velocity[index * 2 + 1] = 0.0;
            }
        }
    }

    // ---------- { Init Textures }----------
    velocityTex = createTextureVec2(velocity, width, heigth);
    const GLuint gridTex = createTextureVec1(grid, width, heigth);
    const GLuint resultsTex = createTextureVec1(results, width, heigth);
    pressureTex = createTextureVec1(pressure, width, heigth);

    // ---------- { Compute program }----------
     projectionProgram = createComputeProgram("../shaders/computes/projection.glsl");
     combineProjProgram  = createComputeProgram("../shaders/computes/combine_proj.glsl");

    glUseProgram(projectionProgram);
    glBindImageTexture (0, velocityTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, resultsTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glUseProgram(combineProjProgram);
    glBindImageTexture (0, velocityTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, resultsTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

fluid2DGpu::~fluid2DGpu() {
    delete[] velocity;
    delete[] grid;
    delete[] results;
    delete[] pressure;
}


void fluid2DGpu::projection(int subStep, float timeStep) const {
    for(int i = 0; i < 1; i ++) {
        glUseProgram(projectionProgram);
        glDispatchCompute(width / 64,heigth / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glUseProgram(combineProjProgram);
        glDispatchCompute(width / 64,heigth / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}

