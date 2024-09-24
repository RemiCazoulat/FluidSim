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
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }
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

Fluid2D::Fluid2D(GLFWwindow * window, const int width, const int height, const int cell_size) : Fluid(window) {
    this->width = width;
    this->height = height;
    this->cell_size = cell_size;
    this->renderer = new Renderer("../shaders/vert2d.glsl", "../shaders/frag2d.glsl");
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}

Fluid2D::~Fluid2D() {
    delete renderer;
    glfwDestroyWindow(window);
    glfwTerminate();
}

