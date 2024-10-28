//
// Created by Rémi on 22/09/2024.
//
#include "../../include/sim/Fluid.h"
Fluid::Fluid(GLFWwindow *window, SimData* simData) {
    this->window = window;
    this->simData = simData;
}

void Fluid::input_step(float dt) {
    if(simData->input_mode == MOUSE) {
        mouse_input(dt);
    } else {
        sound_input(dt);
    }
}
