//
// Created by remi.cazoulat on 11/09/2024.
//

#include "../../../include/sim/2D/Fluid2d.h"

float Fluid2d::force_x = 0.0f;
float Fluid2d::force_y = 0.0f;
float Fluid2d::mouse_x = 0.0f;
float Fluid2d::mouse_y = 0.0f;
int Fluid2d::left_mouse_pressed = 0;
int Fluid2d::right_mouse_pressed = 0;
int Fluid2d::middle_mouse_pressed = 0;


void Fluid2d::mouse_button_callback(GLFWwindow* window, const int button, const int action, int mods) {
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
    if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        middle_mouse_pressed = 1;
    } else if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
        middle_mouse_pressed = 0;
    }
}

void Fluid2d::cursor_position_callback(GLFWwindow* window, const double xpos, const double ypos) {
    mouse_x = static_cast<float>(xpos);
    mouse_y = static_cast<float>(ypos);
}


GLFWwindow* initWindow(const int & windowWidth, const int & windowHeight) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL 2D Fluid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL functions using glfwGetProcAddress
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }
    glfwSwapInterval(0);
    return window;
}

Fluid2d::Fluid2d(const int window_width, const int window_height, const float add_r, const float add_i) {
    this->add_radius = add_r;
    this->add_intensity = add_i;
    this->renderer = new Renderer("../shaders/vert2d.glsl", "../shaders/frag2d.glsl");
    this->window = initWindow(window_width, window_height);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

}

Fluid2d::~Fluid2d() {
    delete renderer;
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Fluid2d::run_loop(const DRAW_MODE mode, const float t_accel) {

    int frame_number = 0;
    double total_time = 0.0;
    double total_sim_time = 0.0;
    double total_rendering_time = 0.0;
    double total_glfw_time = 0.0;
    double previous_time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        const auto current_time = glfwGetTime();
        glfwPollEvents();
        // ----{ Simulation }----
        const auto dt = static_cast<float>((current_time - previous_time) * t_accel);
        previous_time = current_time;
        // start steps
        input_step(add_radius, add_intensity, dt);
        velocity_step(dt);
        density_step(dt);
        GLuint colorTex = draw_step(mode);
        // end steps
        const auto time_sim = glfwGetTime();
        total_sim_time += time_sim - current_time;
        // ----{ Rendering }----
        const auto rendering_time = glfwGetTime();
        // start rendering
        renderer->rendering(colorTex);
        // stop rendering
        total_rendering_time += glfwGetTime() - rendering_time;
        // ----{ Swap Buffers }----
        const auto glfw_time = glfwGetTime();
        // start swap buffers
        glfwSwapBuffers(window);
        // end swap buffers
        total_glfw_time += glfwGetTime() - glfw_time;
        total_time += glfwGetTime() - current_time;
        frame_number++;

    }
    // /////// Debug //////
    debug();
    printf("\n");
    printf("=========[ Main Debug ]=========\n");
    printf("total time: %f s\n", total_time);
    printf("total sim time: %f s (%.2f %%)\n", total_sim_time, total_sim_time / total_time * 100.0);
    printf("total rendering time: %f s (%.2f %%)\n", total_rendering_time, total_rendering_time / total_time * 100.0);
    printf("total glfw time: %f s (%.2f %%)\n", total_glfw_time, total_glfw_time / total_time * 100.0);
    printf("\n");
    printf("total time per frame: %f ms (%.2f FPS)\n", total_time / frame_number * 1000.0, 1.f / (total_time / frame_number));
    printf("total sim time per frame: %f ms\n", total_sim_time / frame_number * 1000.0);
    printf("total rendering time per frame: %f ms\n", total_rendering_time / frame_number * 1000.0);
    printf("total glfw time per frame: %f ms\n", total_glfw_time / frame_number * 1000.0);
}

