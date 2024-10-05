//
// Created by remi.cazoulat on 11/09/2024.
//

#include "../../../include/sim/2D/Flu2D.h"

float Flu2D::force_x = 0.0f;
float Flu2D::force_y = 0.0f;
float Flu2D::mouse_x = 0.0f;
float Flu2D::mouse_y = 0.0f;
int Flu2D::left_mouse_pressed = 0;
int Flu2D::right_mouse_pressed = 0;
int Flu2D::middle_mouse_pressed = 0;


void Flu2D::mouse_button_callback(GLFWwindow* window, const int button, const int action, int mods) {
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


void Flu2D::cursor_position_callback(GLFWwindow* window, const double xpos, const double ypos) {
    mouse_x = static_cast<float>(xpos);
    mouse_y = static_cast<float>(ypos);
}

Flu2D::Flu2D(GLFWwindow * window, SimData* simData) : Fluid(window, simData) {
    this->width = simData->width;
    this->height = simData->height;
    this->renderer = new Renderer("../shaders/vert2d.glsl", "../shaders/frag2d.glsl");
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}

Flu2D::~Flu2D() {
    delete renderer;
    glfwDestroyWindow(window);
    glfwTerminate();
}

