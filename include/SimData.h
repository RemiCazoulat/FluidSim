//
// Created by Rémi on 26/09/2024.
//

#ifndef FLUIDSIM_SIMDATA_H
#define FLUIDSIM_SIMDATA_H
#include "libraries.h"

enum SIM_MODE {
    CPU,
    GL,
    GLo,
    VK,
};

enum SIM_DIM {
    TWO_D,
    THREE_D
};

enum DRAW_MODE {
    VELOCITY,
    DENSITY,
    PRESSURE
};

struct SimData {
    //grid infos
    int resolution = 2; // READONLY
    int real_res = (int)(std::pow(2, resolution));
    int gen_width = 72;
    int gen_height = 72;
    int gen_cell_size = 16;
    int width = gen_width * real_res;
    int height = gen_height * real_res;
    int cell_size = gen_cell_size / real_res;
    float h = 1.f / (float)(height);
    int window_width = gen_width * gen_cell_size;
    int window_height = gen_height * gen_cell_size;
    // Fluid infos
    float diffusion = 0.0001f;
    float viscosity = 0.00000000001f;
    int sub_step = 25;
    // simulation infos
    float time_accel = 1.f;
    SIM_MODE sim_mode = GL; // READONLY
    SIM_DIM sim_dim = TWO_D; // READONLY
    DRAW_MODE draw_mode = VELOCITY;
    // mode
    bool smoke = false;
    bool obstacles = false;
    bool velocity = true;
    // smoke
    float smoke_color[4] = { 1.0f,1.0f,1.0f,1.0f };
    bool smoke_perm = false;
    bool smoke_add = true;
    bool smoke_remove = false;
    float smoke_intensity = 1.0f;
    float smoke_radius = 1.f * (float)(real_res);
    // obstacles
    bool obstacles_add = false;
    bool obstacles_remove = false;
    float obstacles_radius = 1.f * (float)(real_res);
    // velocity
    bool vel_perm = false;
    bool vel_add = true;
    bool vel_remove = false;
    float vel_intensity[3] = {0.0f, 0.0f, 0.0f};
    float ui_vel_radius = 1;
    float vel_radius = ui_vel_radius * (float)(real_res);

    void change_res(int new_res);
    void change_sim_mode(SIM_MODE new_sim_mode);
    void change_sim_dim(SIM_DIM new_sim_dim);
};

#endif //FLUIDSIM_SIMDATA_H

