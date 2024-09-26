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
    int resolution = 3;
    float res_squared = (float)(std::pow(2, resolution));
    int width = static_cast<int>(128.f * res_squared);
    int height = static_cast<int>(72.f * res_squared);
    int cell_size = static_cast<int>(16.f / res_squared);
    // Fluid infos
    float diffusion_rate = 0.0001f;
    float viscosity_rate = 0.00000000001f;
    int sub_step = 25;
    // simulation infos
    SIM_MODE sim_mode = GPU;
    float time_accel = 1.f;
    DRAW_MODE draw_mode = VELOCITY;
    float add_radius = 3 * res_squared;
    float add_intensity_x = 10.f;
    float add_intensity_y = 0.f;
    const float intensities[]{add_intensity_x, add_intensity_y};
    bool smoke = false;
    bool obstacles = false;
    bool velocity = true;
    float smoke_color[4] = { 1.0f,1.0f,1.0f,1.0f };
    bool smoke_perm = false;
    bool smoke_add = true;
    bool smoke_remove = false;
    float smoke_intensity[3] = {0.0f, 0.0f, 0.0f};
    float smoke_radius = 1;
    bool obstacles_add = false;
    bool obstacles_remove = false;
    float obstacles_radius = 1;
    bool vel_perm = false;
    bool vel_add = true;
    bool vel_remove = false;
    float vel_intensity[3] = {0.0f, 0.0f, 0.0f};
    float vel_radius = 1;
};

#endif //FLUIDSIM_SIMDATA_H
