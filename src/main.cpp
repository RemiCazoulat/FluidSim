#include "../../include/shaders/render.h"
#include "../../include/shaders/compute.h"


GLFWwindow* window;
double mouseX, mouseY;
bool leftButtonPressed = false;
bool rightButtonPressed = false;

int gridWidth;
int gridHeight;
int pixelPerCell;

// Init Window, GLFW and GLAD
void initWindow(const int & windowWidth, const int & windowHeight) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create variables

    // Create a GLFW window
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL 2D Fluid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    // Load OpenGL functions using glfwGetProcAddress
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
    // Init grid
    gridWidth = 128 /2;
    gridHeight = 72 /2;
    pixelPerCell = 32;
    const int gridSize = (gridWidth) * (gridHeight);
    const int gridSizex2 = gridSize * 2;
    auto* vel = new GLfloat[gridSizex2]();
    auto* grid = new GLfloat[gridSize]();
    auto* results = new GLfloat[gridSize]();
    auto* density = new GLfloat[gridSize]();
    const auto* densityTransi = new GLfloat[gridSize]();

    printf("[DEBUG] init arrays \n");

    constexpr auto circleCoord = glm::vec2(128 / 2, 72 / 8);
    for(int j = 0; j < gridHeight ; j ++) {
        const int jg = j * gridWidth;
        for(int i = 0; i < gridWidth; i ++) {
            const int index = i + jg;
            if (i == 0 || i == gridWidth - 1 || j == 0 || j == gridHeight - 1) {
                grid[index] = 0.0;
            }
            else {
                grid[index] = 1.0;
            }
            constexpr float radius = 20.0;
            const float distance = glm::distance(glm::vec2(i, j), circleCoord);
            density[index] = distance < radius ? 1.0 : 0.2;

            if( j > gridHeight / 2 - 10 && j < gridHeight / 2 + 10) {
                vel[index * 2] = 1.0;
                vel[index * 2 + 1] = 0.0;
            } else {
                vel[index * 2] = 0.0;
                vel[index * 2 + 1] = 0.0;
            }
        }
    }

    printf("[DEBUG] init arrays values \n");


    // ---------- { Init Window }----------
    const int windowWidth = pixelPerCell * gridWidth;
    const int windowHeight = pixelPerCell * gridHeight;
    printf("[DEBUG] init window size :  %i %i", windowWidth, windowHeight);
    initWindow(windowWidth, windowHeight);
    printf("[DEBUG] init window done \n");

    // ---------- { Init Textures }----------
    const GLuint velTex = createTextureVec2(vel, gridWidth, gridHeight);
    const GLuint gridTex = createTextureVec1(grid, gridWidth, gridHeight);
    const GLuint resultsTex = createTextureVec1(results, gridWidth, gridHeight);

    const GLuint densTex = createTextureVec1(density, gridWidth, gridHeight);

    printf("[DEBUG] init textures done \n");

    // ---------- { Compute program }----------
    const GLuint computeProjection = createComputeProgram("../shaders/computes/projection.glsl");
    printf("[DEBUG] init computeProjection done \n");

    const GLuint computeCombineProj = createComputeProgram("../shaders/computes/combine_proj.glsl");

    printf("[DEBUG] init computes done \n");
    glUseProgram(computeProjection);
    glBindImageTexture (0, velTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, resultsTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    glUseProgram(computeCombineProj);
    glBindImageTexture (0, velTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, gridTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, resultsTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);


    // ---------- { Shader program }----------
    /*
    createGeometry();
    const GLuint shaderProgram = createRenderProgram("../shaders/vShader.shaders","../shaders/fShader.shaders");
    glUseProgram(shaderProgram);
    bindingUniformTex(shaderProgram, "velTex", 0);
    bindingUniformTex(shaderProgram, "densTex", 1);
    printf("[DEBUG] init shader done \n");
    */
    const Render render;
    const GLuint renderProgram = createRenderProgram("../shaders/vert.glsl","../shaders/frag.glsl");
    bindingUniformTex(renderProgram, "velTex", 0);
    bindingUniformTex(renderProgram, "densTex", 1);
    printf("[DEBUG] init shader done \n");



    // ---------- { Main render loop }----------
    while (!glfwWindowShouldClose(window)) {
        for(int i = 0; i < 1; i ++) {
            glUseProgram(computeProjection);
            glDispatchCompute(gridWidth / 64,gridHeight / 1,1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            glUseProgram(computeCombineProj);
            glDispatchCompute(gridWidth / 64,gridHeight / 1,1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        /* TODO : Appeler le shader advection pour calculer la densité finale afin de voir qql chose à l'affichage
         * ou faire l'affichage en fonction de la velocité
         *
         */

        render.makeRender(renderProgram, velTex, densTex, VEL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Clean up
    render.cleanRender(renderProgram);
    cleanCompute(computeProjection);
    cleanCompute(computeCombineProj);
    glfwDestroyWindow(window);
    glfwTerminate();

    delete[] vel;
    delete[] density;

    return 0;
}
