#include "../../include/shaders/Renderer.h"
#include "../include/sim/2D/CpuFluid2D.h"
#include "../include/sim/2D/GlFluid2D.h"
#include "../include/sim/2D/GlFluid2DOpti.h"
#include "../include/ui/Interface.h"

GLFWwindow* initWindow(const int width, const int height, const int cell_size) {
    const int window_width = width * cell_size;
    const int window_height = height * cell_size;
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "OpenGL 2D Fluid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    // Load OpenGL functions using glfwGetProcAddress
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }
    return window;
}

int main() {

    // ----{ init functions }----
    auto* simData = new SimData;
    GLFWwindow* window = initWindow(simData->width, simData->height, simData->cell_size);

    // ----{ Choosing simulation }----
    Fluid* fluid;
    if (simData->sim_mode == CPU) {
        fluid = new CpuFluid2D(window, simData);
    } else {
        fluid = new GlFluid2D(window, simData);
    }
    auto* interface = new Interface(window, simData);

    // ----{ Main Loop }----
    int frame_number = 0;
    double total_time = 0.0;
    double frame_time = 0.0;
    while (!glfwWindowShouldClose(window)) {
        // time management
        const auto current_time = glfwGetTime();
        const float dt = (float)(frame_time) * simData->time_accel;

        // ----{ Simulation }----
        glfwPollEvents();
        fluid->input_step(dt);
        fluid->velocity_step(dt);
        fluid->density_step(dt);
        fluid->draw_step(simData->draw_mode);
        // ----{ Ui }----
        interface->initFrame();
        interface->runInputWindow();
        interface->runSimulationWindow();
        interface->runMenuWindow();
        interface->runDebugWindow(dt, frame_number, total_time);
        interface->runColorWheelWindow();
        interface->renderFrame();

        // swap buffers
        glfwSwapBuffers(window);

        // time management
        frame_time = glfwGetTime() - current_time;
        total_time += glfwGetTime() - current_time;
        frame_number++;
    }
    // ----{ Clean Up }----
    delete simData;
    delete interface;
    delete fluid;
    return 0;
}
