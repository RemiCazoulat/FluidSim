#include "../../include/shaders/render.h"
#include "../include/sim/obstacleFlu.h"
#include "../../include/sim/fluGpu.h"


enum SIM_MODE {
    CPU,
    GPU
};

GLFWwindow* window;
int width;
int height;
int cell_size;

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
    // Load OpenGL functions using glfwGetProcAddress
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }
}

int main() {
// /////////// Control Panel ////////
/**/// grid infos
/**/constexpr float res = 2.f;
/**/width = static_cast<int>(128.f * res);
/**/height = static_cast<int>(72.f * res);
/**/cell_size = static_cast<int>(16.f / res);
/**/// fluid infos
/**/constexpr float diffusion_rate = 0.0001f;
/**/constexpr float viscosity_rate = 0.00000001f;
/**/constexpr int sub_step = 25;
/**/// simulation infos
/**/constexpr SIM_MODE sim_mode = GPU;
/**/constexpr float time_accel = 0.1f;
/**/constexpr DRAW_MODE draw_mode = VELOCITY;
/**/const int add_radius = 5 * res;
/**/constexpr float add_intensity = 0.5f;
// /////////// End of control Panel ////////


    // /////// Init Window ///////
    const int window_width = cell_size * width;
    const int window_height = cell_size * height;
    initWindow(window_width, window_height);

    // /////// Render program ///////
    const Render render;
    const GLuint renderProgram = createRenderProgram("../shaders/vert.glsl","../shaders/frag.glsl");
    bindingUniformTex(renderProgram, "colorTex", 0);

    // ////// Main loop ///////
    fluid* fluid;
    if constexpr (sim_mode == CPU) {
        fluid = new obstacleFlu(window, width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }
    else {
        fluid = new fluGpu(window, width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }
    auto previousTime = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        const auto currentTime = static_cast<float>(glfwGetTime());
        const auto dt = (currentTime - previousTime) * time_accel;
        previousTime = currentTime;

        fluid->input_step(add_radius, add_intensity, dt);
        fluid->velocity_step(dt);
        fluid->density_step(dt);

        if constexpr (draw_mode == PRESSURE) {
            fluid->calculate_pressure(dt);
        }
        GLuint colorTex = fluid->draw(draw_mode);
        render.makeRender(renderProgram, colorTex);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // /////// Clean up ///////
    delete fluid;
    render.cleanRender(renderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
