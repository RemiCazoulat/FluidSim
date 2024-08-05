#include "shader.h"
#include "compute.h"

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

GLFWwindow* window;
double mouseX, mouseY;
bool leftButtonPressed = false;
bool rightButtonPressed = false;

int gridWidth;
int gridHeight;
int pixelPerCell;

// Callback pour la gestion des clics de souris
void mouse_button_callback(const int button, const int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        leftButtonPressed = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        rightButtonPressed = (action == GLFW_PRESS);
    }
}

// Callback pour la gestion de la position de la souris
void cursor_position_callback(const double xpos, const double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}

// Conversion des coordonnées écran en coordonnées texture
glm::ivec2 screenToTextureCoordinates(double x, double y) {
    const int windowWidth = gridWidth * pixelPerCell;
    const int windowHeight = gridHeight * pixelPerCell;
    const int texX = static_cast<int>((x / windowWidth) * gridWidth);
    const int texY = static_cast<int>((1.0 - (y / windowHeight)) * gridHeight);
    return { texX, texY };
}

// Fonction pour ajouter de la vélocité
void addVelocity(const GLuint computeShader, const double x, const double y) {
    glm::ivec2 texCoord = screenToTextureCoordinates(x, y);

    glUseProgram(computeShader);
    glUniform2f(glGetUniformLocation(computeShader, "mousePos"), texCoord.x, texCoord.y);
    glUniform1i(glGetUniformLocation(computeShader, "addVelocity"), 1);
    glUseProgram(0);
}

// Fonction pour ajouter de la densité
void addDensity(const GLuint computeShader, const double x, const double y) {
    glm::ivec2 texCoord = screenToTextureCoordinates(x, y);

    glUseProgram(computeShader);
    glUniform2f(glGetUniformLocation(computeShader, "mousePos"), texCoord.x, texCoord.y);
    glUniform1i(glGetUniformLocation(computeShader, "addDensity"), 1);
    glUseProgram(0);
}

// Initialisation de la fenêtre, GLFW et GLAD
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

// Création d'une texture pour un vecteur 2D
GLuint createTextureVec2(const GLfloat* data, const int width, const int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, data);
    return texture;
}

// Création d'une texture pour un vecteur 1D
GLuint createTextureVec1(const GLfloat* data, const int width, const int height) {
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

int main() {
    gridWidth = 64; // 128 / 2
    gridHeight = 36; // 72 / 2
    pixelPerCell = 32;
    const int gridSize = gridWidth * gridHeight;
    const int gridSizex2 = gridSize * 2;

    std::vector<GLfloat> vel(gridSizex2, 0.0f);
    std::vector<GLfloat> density(gridSize, 0.0f);
    const std::vector<GLfloat> densityTransi(gridSize, 0.0f);

    std::cout << "[DEBUG] init arrays \n";

    glm::vec2 circleCoord(64, 9); // 128 / 2, 72 / 8
    float radius = 20.0f;

    for (int j = 0; j < gridHeight; ++j) {
        for (int i = 0; i < gridWidth; ++i) {
            float distance = glm::distance(glm::vec2(i, j), circleCoord);
            density[i + j * gridWidth] = distance < radius ? 1.0f : 0.2f;

            if (j > gridHeight / 2 - 10 && j < gridHeight / 2 + 10) {
                vel[(i + j * gridWidth) * 2] = 1.0f;
                vel[(i + j * gridWidth) * 2 + 1] = 0.0f;
            }
        }
    }

    std::cout << "[DEBUG] init arrays values \n";

    const int windowWidth = pixelPerCell * gridWidth;
    const int windowHeight = pixelPerCell * gridHeight;
    std::cout << "[DEBUG] init window size: " << windowWidth << " " << windowHeight << "\n";
    initWindow(windowWidth, windowHeight);
    std::cout << "[DEBUG] init window done \n";

    const GLuint velTex = createTextureVec2(vel.data(), gridWidth, gridHeight);
    const GLuint densTex = createTextureVec1(density.data(), gridWidth, gridHeight);
    const GLuint densTexTransit = createTextureVec1(densityTransi.data(), gridWidth, gridHeight);
    std::cout << "[DEBUG] init textures done \n";

    const GLuint computeProgram = createComputeProgram("../glsl/cPhysics.glsl");
    const GLuint computeReplace = createComputeProgram("../glsl/cReplace.glsl");
    std::cout << "[DEBUG] init compute done \n";

    glUseProgram(computeProgram);
    glBindImageTexture(0, velTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture(1, densTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, densTexTransit, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glUseProgram(computeReplace);
    glBindImageTexture(0, densTexTransit, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, densTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    createGeometry();
    const GLuint shaderProgram = createShaderProgram("../glsl/vShader.glsl", "../glsl/fShader.glsl");
    glUseProgram(shaderProgram);
    bindingUniformTex(shaderProgram, "velTex", 0);
    bindingUniformTex(shaderProgram, "densTex", 1);
    std::cout << "[DEBUG] init shader done \n";

    while (!glfwWindowShouldClose(window)) {
        glUseProgram(computeProgram);
        for (int i = 0; i < 200; ++i) {
            glDispatchCompute(gridWidth / 64, gridHeight / 1, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        glUseProgram(computeReplace);
        glDispatchCompute(gridWidth / 64, gridHeight / 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        render(shaderProgram, velTex, densTex);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanShader(shaderProgram);
    cleanCompute(computeProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
