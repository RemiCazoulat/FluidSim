#include "../../include/shaders/renderer.h"
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
/**/constexpr float res = 4.f;
/**/width = static_cast<int>(128.f * res);
/**/height = static_cast<int>(72.f * res);
/**/cell_size = static_cast<int>(16.f / res);
/**/// fluid infos
/**/constexpr float diffusion_rate = 0.0001f;
/**/constexpr float viscosity_rate = 0.0000000001f;
/**/constexpr int sub_step = 50;
/**/// simulation infos
/**/constexpr SIM_MODE sim_mode = GPU;
/**/constexpr float time_accel = 1.f;
/**/constexpr DRAW_MODE draw_mode = VELOCITY;
/**/const int add_radius = 7 * res;
/**/constexpr float add_intensity = 0.5f;
// /////////// End of control Panel ////////

    // /////// Init Window ///////
    const int window_width = cell_size * width;
    const int window_height = cell_size * height;
    initWindow(window_width, window_height);

    // /////// Render program ///////
    const renderer* render = new renderer("../shaders/vert.glsl", "../shaders/frag.glsl");

    // ////// Choosing simulation ///////
    fluid* fluid;
    if constexpr (sim_mode == CPU) {
        fluid = new obstacleFlu(window, width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }
    else {
        fluid = new fluGpu(window, width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }
    // ////// Main loop ///////
    int frame_number = 0;
    double total_time = 0.0;
    double total_sim_time = 0.0;
    double previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        // Calculating dt
        const auto currentTime = glfwGetTime();
        const auto dt = static_cast<float>((currentTime - previousTime) * time_accel);
        previousTime = currentTime;
        // /////// Simulation ///////
        fluid->input_step(add_radius, add_intensity, dt);
        fluid->velocity_step(dt);
        fluid->density_step(dt);
        const auto time_sim = glfwGetTime();
        total_sim_time += time_sim - currentTime;
        if constexpr (draw_mode == PRESSURE) {
            fluid->pressure_step(dt);
        }
        // drawing & rendering
        GLuint colorTex = fluid->draw_step(draw_mode);
        render->rendering(colorTex);

        glfwSwapBuffers(window);
        glfwPollEvents();
        total_time += dt;
        frame_number++;

    }
    // /////// Debug //////
    fluid->debug();
    printf("\n");
    printf("total time: %f s\n", total_time);
    printf("total sim time: %f s (%.2f %%)\n", total_sim_time, total_sim_time / total_time * 100.0);
    printf("drawing time: %f s (%.2f %%)\n", total_time - total_sim_time, (total_time - total_sim_time) / total_time * 100.0);
    printf("\n");
    printf("total time per frame: %f ms\n", total_time / frame_number * 1000.0);
    printf("total sim time per frame: %f ms\n", total_sim_time / frame_number * 1000.0);
    printf("drawing time per frame: %f ms\n", (total_time - total_sim_time) / frame_number * 1000.0);
    // /////// Clean up ///////
    delete fluid;
    delete render;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
