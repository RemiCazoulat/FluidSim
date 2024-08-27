#include "../../include/shaders/render.h"
#include "../../include/shaders/compute.h"
#include "../include/sim/fluid2DGpu.h"
#include "../include/sim/fluid2DCpu.h"
#include "../include/sim/stable_fluid.h"


//#define USE_GPU

GLFWwindow* window;
float force_x = 0.0f, force_y = 0.0f, mouse_x = 0.0f, mouse_y = 0.0f;
int mouse_pressed = 0;

int width;
int height;
int cell_size;



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse_pressed = 1;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouse_pressed = 0;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouse_x = static_cast<float>(xpos);
    mouse_y = static_cast<float>(ypos);
}

void initWindow(const int & windowWidth, const int & windowHeight) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create a GLFW window
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL 2D Fluid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    // Load OpenGL functions using glfwGetProcAddress
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }
}

// -------------------------------------------------
// -----------{ Main function }---------------------
// -------------------------------------------------
int main() {
    // Init grid
    width = 128 * 2 ;
    height = 72 * 2;
    cell_size = 16 / 2;

    // ---------- { Init Window }----------
    const int window_width = cell_size * width;
    const int window_height = cell_size * height;
    printf("init window size :  %i %i", window_width, window_height);
    initWindow(window_width, window_height);

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
    /*
    const auto* fluid = new fluid2DCpu(gridWidth, gridHeight, 0.1);
    while (!glfwWindowShouldClose(window)) {
        delta_time = 1.f / 600000;
        fluid->compute_gravity(delta_time);
        fluid->projection(1, delta_time, 1.1);
        fluid->calculate_pressure_color();
        GLuint pressureColorTex = createTextureVec1(fluid->pressure_color, gridWidth, gridHeight);
        GLuint isBorderTex = createTextureVec1(fluid->is_border, gridWidth, gridHeight);
        render.makeRender(renderProgram, pressureColorTex, isBorderTex);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete fluid;
    */
    auto* fluid = new stable_fluid(width, height, cell_size, 1, 1);
    //auto previousTime = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        constexpr float dt = 1.f / 60;
        fluid->watch_inputs(mouse_pressed, mouse_x, mouse_y, force_x, force_y);
        fluid->velocity_step(dt);
        fluid->density_step(dt);
        fluid->draw(VELOCITY);
        GLuint colorTex = createTextureVec3(fluid->color, width, height);
        render.makeRender(renderProgram, colorTex);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete fluid;
#endif

    render.cleanRender(renderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
