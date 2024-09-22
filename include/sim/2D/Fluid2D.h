//
// Created by remi.cazoulat on 29/08/2024.
//

#ifndef FLUID2D_H
#define FLUID2D_H

#include "../Fluid.h"
#include "../../shaders/Renderer.h"


class Fluid2D : public Fluid {
protected:
    Renderer *renderer;
    static float force_x, force_y , mouse_x, mouse_y;
    static int left_mouse_pressed, right_mouse_pressed , middle_mouse_pressed;
    int width;
    int height;
    int cell_size;
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
public:
    explicit Fluid2D( GLFWwindow* window, int width, int height, int cell_size);
    ~Fluid2D() override;
};

#endif //FLUID2D_H
