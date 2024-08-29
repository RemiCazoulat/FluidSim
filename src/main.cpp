#include "../../include/shaders/render.h"
#include "../../include/shaders/compute.h"
#include "../include/sim/fluid2DGpu.h"
#include "../include/sim/fluid2DCpu.h"
#include "../include/sim/stable_fluid.h"


//#define USE_GPU

GLFWwindow* window;
float force_x = 0.0f, force_y = 0.0f, mouse_x = 0.0f, mouse_y = 0.0f;
int left_mouse_pressed = 0, right_mouse_pressed = 0;

int width;
int height;
int cell_size;



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouse_x = static_cast<float>(xpos);
    mouse_y = static_cast<float>(ypos);
}

void watch_inputs(const stable_fluid *fluid)  {

    if (left_mouse_pressed || right_mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / cell_size;
        const int j = static_cast<int>((static_cast<float>(cell_size * height) - mouse_y)) / cell_size;

        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            if(left_mouse_pressed) {
                fluid->add_vel(i, j, (mouse_x - force_x) * 100.f, (mouse_y - force_y) * 100.f);
                force_x = mouse_x;
                force_y = mouse_y;
            }
            if(right_mouse_pressed) {
                fluid->add_dens(i, j);
            }
        }
    }
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
    constexpr float res = 4.f;
    width = static_cast<int>(128.f * res);
    height = static_cast<int>(72.f * res);
    cell_size = static_cast<int>(16.f / res);

    // ---------- { Init Window }----------
    const int window_width = cell_size * width;
    const int window_height = cell_size * height;
    initWindow(window_width, window_height);

    // ---------- { Render program }----------
    const Render render;
    const GLuint renderProgram = createRenderProgram("../shaders/vert.glsl","../shaders/frag.glsl");
    bindingUniformTex(renderProgram, "velTex", 0);
    bindingUniformTex(renderProgram, "densTex", 1);

#ifdef USE_GPU
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
    auto* fluid = new stable_fluid(width, height, cell_size, 0.0001, 0.000001, 20);
    //auto previousTime = static_cast<float>(glfwGetTime());
    int i = 0;
    while (!glfwWindowShouldClose(window)) {
        //printf("====== Frame %d ======\n", i++);
        constexpr float dt = 1.f / 60;

        watch_inputs(fluid);
        fluid->add_all_perm_step();
        fluid->velocity_step(dt);
        fluid->density_step(dt);
        fluid->draw(DENSITY);

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
