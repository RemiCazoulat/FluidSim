//
// Created by remi.cazoulat on 29/08/2024.
//

#ifndef FLUID_2D_H
#define FLUID_2D_H

#include "../../include/sim/fluid.h"
#include "../../include/shaders/renderer.h"


class fluid_2d : public fluid {
    float add_radius;
    float add_intensity;
    renderer *render;
protected:
    static float force_x, force_y , mouse_x, mouse_y;
    static int left_mouse_pressed, right_mouse_pressed , middle_mouse_pressed;
    GLFWwindow *window;
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
public:
    explicit fluid_2d(int window_width, int window_height, float add_r, float add_i);
    ~fluid_2d() override;
    void run_loop(DRAW_MODE mode, float t_accel) override;
};

#endif //FLUID_2D_H
