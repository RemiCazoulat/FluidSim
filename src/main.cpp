#include "../../include/shaders/Renderer.h"
#include "../include/sim/2D/CpuFluid2D.h"
#include "../include/sim/2D/GlFluid2D.h"
#include "../include/sim/2D/GlFluid2DOpti.h"

enum SIM_MODE {
    CPU,
    GPU
};

int main() {

    // grid infos
    constexpr float res = 8.f;
    const int width = static_cast<int>(128.f * res);
    const int height = static_cast<int>(72.f * res);
    const int cell_size = static_cast<int>(16.f / res);
    // Fluid infos
    constexpr float diffusion_rate = 0.0001f;
    constexpr float viscosity_rate = 0.00000000001f;
    constexpr int sub_step = 25;
    // simulation infos
    constexpr SIM_MODE sim_mode = GPU;
    constexpr float time_accel = 1.f;
    constexpr DRAW_MODE draw_mode = VELOCITY;
    const int add_radius = 3 * res;
    constexpr float add_intensity_x = 10.f;
    constexpr float add_intensity_y = 0.f;
    const float intensities[]{add_intensity_x, add_intensity_y};


    printf("Parameters set. \n");

    // ----{ Choosing simulation }----
    Fluid* fluid;
    if constexpr (sim_mode == CPU) {
        fluid = new CpuFluid2D(width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }
    else {
        fluid = new GlFluid2D(width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }
    // ----{ Main Loop }----
    fluid->run_loop(draw_mode, time_accel, add_radius, intensities);
    // ----{ Clean Up }----
    delete fluid;
    return 0;
}
