//
// Created by remi.cazoulat on 20/08/2024.
//

#include "../../include/sim/fluid2DGpu.h"

GLuint createTextureVec2(const GLfloat * data, const int width, const int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, data);
    return texture;
}
GLuint createTextureVec1(const GLfloat * data, const int width, const int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}



fluid2DGpu::fluid2DGpu(const int width, const int heigth, const int pixelsPerCell, const float fluidDensity) {
    this->width = width;
    this->heigth = heigth;
    this->pixelsPerCell = pixelsPerCell;
    this->fluidDensity = fluidDensity;
    const int gridSize = width * heigth;
    const int gridSizex2 = gridSize * 2;
    this->vel = new GLfloat[gridSizex2]();
    this->grid = new GLfloat[gridSize]();
    this->results = new GLfloat[gridSize]();
    this->pressure = new GLfloat[gridSize]();

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
                vel[index * 2] = 1.0;
                vel[index * 2 + 1] = 0.0;
            } else {
                vel[index * 2] = 0.0;
                vel[index * 2 + 1] = 0.0;
            }
        }
    }

    // ---------- { Init Textures }----------
    velocityTex = createTextureVec2(vel, width, heigth);
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
