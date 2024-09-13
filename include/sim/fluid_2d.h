//
// Created by remi.cazoulat on 29/08/2024.
//

#ifndef FLUID_2D_H
#define FLUID_2D_H

#include "../../include/sim/fluid.h"


class fluid_2d : public fluid {
protected:
    static float force_x, force_y , mouse_x, mouse_y;
    static int left_mouse_pressed, right_mouse_pressed , middle_mouse_pressed ;
public:
    explicit fluid_2d(GLFWwindow* window);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
};

#endif //FLUID_2D_H
