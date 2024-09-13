//
// Created by remi.cazoulat on 13/09/2024.
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
    virtual void input_step(float r, float intensity, float dt) = 0;
    virtual void density_step(float dt) = 0;
    virtual void velocity_step(float dt) = 0;
    virtual void pressure_step(float dt) = 0;
    [[nodiscard]]virtual GLuint draw_step(DRAW_MODE mode) = 0;
public:
    virtual ~fluid() = default;
    virtual void run_loop(DRAW_MODE mode, float t_accel) = 0;

    virtual void debug() = 0;

};
#endif //FLUID_H
