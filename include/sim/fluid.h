//
// Created by remi.cazoulat on 29/08/2024.
//

#ifndef FLUID_H
#define FLUID_H

enum DRAW_MODE {
    VELOCITY = 0,
    DENSITY = 1,
    PRESSURE = 2
};
class fluid {
public:
    virtual ~fluid() = default;

    virtual void input_step(const float r, const float intensity, const float dt) = 0;
    virtual void density_step(float dt) = 0;
    virtual void velocity_step(float dt) = 0;
    virtual void pressure_step(float dt) = 0;
    [[nodiscard]]virtual GLuint draw_step(DRAW_MODE mode) = 0;
    virtual void debug() = 0;


};

#endif //FLUID_H
