//
// Created by remi.cazoulat on 11/09/2024.
//

#include "../../include/sim/fluid_2d.h"

float fluid_2d::force_x = 0.0f;
float fluid_2d::force_y = 0.0f;
float fluid_2d::mouse_x = 0.0f;
float fluid_2d::mouse_y = 0.0f;
int fluid_2d::left_mouse_pressed = 0;
int fluid_2d::right_mouse_pressed = 0;
int fluid_2d::middle_mouse_pressed = 0;


void fluid_2d::mouse_button_callback(GLFWwindow* window, const int button, const int action, int mods) {
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

void fluid_2d::cursor_position_callback(GLFWwindow* window, const double xpos, const double ypos) {
    mouse_x = static_cast<float>(xpos);
    mouse_y = static_cast<float>(ypos);
}

fluid_2d::fluid_2d(GLFWwindow* window) {
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

}

