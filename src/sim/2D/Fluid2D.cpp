//
// Created by remi.cazoulat on 11/09/2024.
//

#include "../../../include/sim/2D/Fluid2D.h"

float Fluid2D::force_x = 0.0f;
float Fluid2D::force_y = 0.0f;
float Fluid2D::mouse_x = 0.0f;
float Fluid2D::mouse_y = 0.0f;
int Fluid2D::left_mouse_pressed = 0;
int Fluid2D::right_mouse_pressed = 0;
int Fluid2D::middle_mouse_pressed = 0;


void Fluid2D::mouse_button_callback(GLFWwindow* window, const int button, const int action, int mods) {
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

void Fluid2D::cursor_position_callback(GLFWwindow* window, const double xpos, const double ypos) {
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

Fluid2D::Fluid2D(const int width, const int height, const int cell_size) {
    this->width = width;
    this->height = height;
    this->cell_size = cell_size;
    const int window_width = width * cell_size;
    const int window_height = height * cell_size;
    this->window = initWindow(window_width, window_height);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    this->renderer = new Renderer("../shaders/vert2d.glsl", "../shaders/frag2d.glsl");
}

Fluid2D::~Fluid2D() {
    delete renderer;
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Fluid2D::run_loop(const DRAW_MODE mode, const float t_accel, const float r, const float* intensities) {
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
        input_step(r, intensities, dt);
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
    // ----{ Debug }----
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
