//
// Created by remi.cazoulat on 29/08/2024.
//

#ifndef FLUID_H
#define FLUID_H

enum DRAW_MODE {
    DENSITY,
    VELOCITY,
    PRESSURE
};
class fluid {
public:
    virtual ~fluid() = default;

    virtual void inputs_step(int r, float intensity) const = 0;
    virtual void density_step(float dt) = 0;
    virtual void velocity_step(float dt) = 0;
    virtual void calculate_pressure(float dt) const = 0;
    [[nodiscard]]virtual GLuint draw(DRAW_MODE mode) const = 0;


};

#endif //FLUID_H
