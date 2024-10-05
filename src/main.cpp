#include "../../include/shaders/Renderer.h"
#include "../include/sim/2D/cpuFlu2D.h"
#include "../include/sim/2D/GLFlu2D.h"
#include "../include/sim/2D/GLoFlu2D.h"
#include "../include/ui/Interface.h"

void initGlfw() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
GLFWwindow* initWindow(const int w_width, const int w_height) {
    GLFWwindow *window = glfwCreateWindow(w_width, w_height, "Smoker Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    return window;
}
void initOpenGL() {
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }
}

int main() {

    // ----{ init functions }----
    initGlfw();
    auto* simData = new SimData;
    GLFWwindow* window = initWindow(simData->window_width, simData->window_height);
    initOpenGL();

    // ----{ Choosing simulation }----
    constexpr int fluid_nbr = 3;
    Fluid* fluids[fluid_nbr];

    // according to enum SIM_MODE in SimData.h file
    fluids[0] = new cpuFlu2D(window, simData);
    fluids[1] = new GLFlu2D(window, simData);
    fluids[2] = new GLoFlu2D(window, simData);


    Interface* interface;
    interface = new Interface(window, simData);

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
        fluids[simData->sim_mode]->input_step(dt);
        fluids[simData->sim_mode]->velocity_step(dt);
        fluids[simData->sim_mode]->density_step(dt);
        fluids[simData->sim_mode]->draw_step(simData->draw_mode);
        // ----{ UI }----
        interface->initFrame();
        interface->runInputWindow();
        interface->runMenuWindow();
        interface->runDebugWindow(dt, frame_number, total_time);
        interface->runColorWheelWindow();
        interface->runSimulationWindow();
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
    for(Fluid* & fluid : fluids) {
        delete fluid;
    }
    return 0;
}
