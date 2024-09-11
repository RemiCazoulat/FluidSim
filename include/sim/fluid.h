//
// Created by remi.cazoulat on 29/08/2024.
//

#ifndef FLUID_H
#define FLUID_H

#include "../../include/libraries.h"

enum DRAW_MODE {
    VELOCITY = 0,
    DENSITY = 1,
    PRESSURE = 2
};
class fluid {

protected:
   static float force_x, force_y , mouse_x, mouse_y;
   static int left_mouse_pressed, right_mouse_pressed , middle_mouse_pressed ;
public:

    explicit fluid(GLFWwindow* window);
    virtual ~fluid() = default;
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

    virtual void input_step(float r, float intensity, float dt) = 0;
    virtual void density_step(float dt) = 0;
    virtual void velocity_step(float dt) = 0;
    virtual void pressure_step(float dt) = 0;
    [[nodiscard]]virtual GLuint draw_step(DRAW_MODE mode) = 0;
    virtual void debug() = 0;


};

#endif //FLUID_H
