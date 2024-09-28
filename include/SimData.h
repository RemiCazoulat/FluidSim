//
// Created by Rémi on 26/09/2024.
//

#ifndef FLUIDSIM_SIMDATA_H
#define FLUIDSIM_SIMDATA_H

enum SIM_MODE {
    CPU,
    GPU
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
    int resolution = 3;
    float real_res = (float)(std::pow(2, resolution));
    int width = static_cast<int>(128.f * real_res);
    int height = static_cast<int>(72.f * real_res);
    int cell_size = static_cast<int>(16.f / real_res);
    float h_w = 1.f / (float)(width);
    float h_h = 1.f / (float)(height);
    int window_width = width * cell_size;
    int window_height = height * cell_size;
    // Fluid infos
    float diffusion = 0.0001f;
    float viscosity = 0.00000000001f;
    int sub_step = 25;
    // simulation infos
    float time_accel = 1.f;
    SIM_MODE sim_mode = GPU;
    DRAW_MODE draw_mode = VELOCITY;
    SIM_DIM sim_dim = TWO_D;
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
    float smoke_radius = 1 * real_res;
    // obstacles
    bool obstacles_add = false;
    bool obstacles_remove = false;
    float obstacles_radius = 1 * real_res;
    // velocity
    bool vel_perm = false;
    bool vel_add = true;
    bool vel_remove = false;
    float vel_intensity[3] = {0.0f, 0.0f, 0.0f};
    float vel_radius = 1 * real_res;
};

#endif //FLUIDSIM_SIMDATA_H
