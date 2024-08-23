#include "../../include/shaders/render.h"
#include "../../include/shaders/compute.h"
#include "../include/sim/fluid2DGpu.h"
#include "../include/sim/fluid2DCpu.h"


//#define USE_GPU


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

    // ---------- { Init Window }----------
    const int windowWidth = pixelPerCell * gridWidth;
    const int windowHeight = pixelPerCell * gridHeight;
    printf("init window size :  %i %i", windowWidth, windowHeight);
    initWindow(windowWidth, windowHeight);


    // ---------- { Render program }----------
    const Render render;
    const GLuint renderProgram = createRenderProgram("../shaders/vert.glsl","../shaders/frag.glsl");
    bindingUniformTex(renderProgram, "velTex", 0);
    bindingUniformTex(renderProgram, "densTex", 1);
    printf("[DEBUG] init shader done \n");



#ifdef USE_GPU
    // ---------- { GPU }----------
    const auto* fluid = new fluid2DGpu(gridWidth, gridHeight, pixelPerCell, 1.0);
    while (!glfwWindowShouldClose(window)) {
        fluid->projection(10, 0.1);
        render.makeRender(renderProgram, fluid->velocityTex, fluid->pressureTex, VEL);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete fluid;
#else
    // ---------- { CPU }----------
    const auto* fluid = new fluid2DCpu(gridWidth, gridHeight, pixelPerCell, 1.0);
    auto previousTime = static_cast<float>(glfwGetTime());
    float currentTime = 0.0;
    float deltaTime = 0.0;
    while (!glfwWindowShouldClose(window)) {
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        fluid->compute_gravity(0.1);
        fluid->projection(10, deltaTime, 1.9);
        GLuint velocityTex = createTextureVec2(fluid->velocity, gridWidth, gridHeight);
        GLuint pressureTex = createTextureVec1(fluid->pressure, gridWidth, gridHeight);
        render.makeRender(renderProgram, velocityTex, pressureTex, VEL);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete fluid;
#endif
    // Clean up
    render.cleanRender(renderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
