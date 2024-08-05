#include "shader.h"
#include "compute.h"
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>


GLFWwindow* window;

void initWindow(const int& windowWidth, const int& windowHeight) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL 2D Fluid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }
}


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
// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------
// -----------{ Main function }--------------------------
// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------
// ---{ M }---
// ---{ A }---
// ---{ I }---
// ---{ N }---
int main() {
    constexpr int matrixSize = 3;
    // Matrix ID
    std::vector matrixID(matrixSize * matrixSize, 0.0f);
    for(int i = 0; i < matrixSize; i++) {
        for(int j = 0; j < matrixSize; j++) {
            matrixID[i + j * matrixSize] = i == j ? 1.0f : 0.0f;
        }
    }

    //Data
    const std::vector<GLfloat> matrix = {
    3, -2, 4,
    2, -4, 5,
    1,  8, 2
    };

    // ---------- { Init Textures }-----------
    initWindow(10, 10);

    const GLuint matrixTex = createTextureVec1(matrix.data(), matrixSize, matrixSize);
    const GLuint matrixIDTex = createTextureVec1(matrixID.data(), matrixSize, matrixSize);


    // ---------- { Compute program }----------
    const GLuint computeMatrix = createComputeProgram("../glsl/cMatrixInversion.glsl");

    glBindImageTexture (0, matrixTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (1, matrixIDTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    GLint normalizeLoc = glGetUniformLocation(computeMatrix, "normalize");
    GLint matrixSizeLoc = glGetUniformLocation(computeMatrix, "matrixSize");
    GLint iiLoc = glGetUniformLocation(computeMatrix, "ii");
    GLint currentLineLoc = glGetUniformLocation(computeMatrix, "currentLine");
    GLint lineToSubLoc = glGetUniformLocation(computeMatrix, "lineToSub");

    //glUniform1f(floatLocation, floatValue);
    //glUniform1i(boolLocation, boolValue);
    //glUniform1i(intLocation, intValue);

    glUseProgram(computeMatrix);
    glUniform1i(matrixSizeLoc, matrixSize);

    for(int i = 0; i < matrixSize; i ++) {
        glUniform1i(normalizeLoc, true);
        glUniform1f(iiLoc, matrix[i + i * matrixSize]);
        glUniform1i(currentLineLoc, i);

        glDispatchCompute(matrixSize * 2, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glUniform1i(normalizeLoc, false);
        for(int j = 0; j < matrixSize; j ++) {
            if(j != i) {
                glUniform1i(lineToSubLoc, j);
                glDispatchCompute(matrixSize, 1, 1);
                //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            }
        }
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    std::vector<float> outputMatrix;
    std::vector<float> outputMatrixID;

    glBindTexture(GL_TEXTURE_2D, matrixTex);
    outputMatrix.resize(matrixSize * matrixSize ); // Assume RGB format
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, outputMatrix.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, matrixIDTex);
    outputMatrixID.resize(matrixSize * matrixSize ); // Assume RGB format
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, outputMatrixID.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    printf("Matrix ID:\n");
    for(int i = 0; i < matrixSize; i++) {
        for(int j = 0; j < matrixSize; j++) {
            printf("%f ", outputMatrixID[i + j * matrixSize]);
        }
        printf("\n");
    }

    printf("Matrix:\n");
    for(int i = 0; i < matrixSize; i++) {
        for(int j = 0; j < matrixSize; j++) {
            printf("%f ", outputMatrix[i + j * matrixSize]);
        }
        printf("\n");
    }







    return 0;
}