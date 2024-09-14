//
// Created by remi.cazoulat on 29/08/2024.
//

#ifndef FLUID_2D_H
#define FLUID_2D_H

#include "../Fluid.h"
#include "../../shaders/Renderer.h"


class Fluid2d : public Fluid {
    float add_radius;
    float add_intensity;
    Renderer *renderer;
protected:
    static float force_x, force_y , mouse_x, mouse_y;
    static int left_mouse_pressed, right_mouse_pressed , middle_mouse_pressed;
    GLFWwindow *window;
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
public:
    explicit Fluid2d(int window_width, int window_height, float add_r, float add_i);
    ~Fluid2d() override;
    void run_loop(DRAW_MODE mode, float t_accel) override;
};

#endif //FLUID_2D_H
