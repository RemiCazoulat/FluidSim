#include "../../include/shaders/Renderer.h"
#include "../include/sim/2D/CpuFluid2d.h"
#include "../include/sim/2D/GlFluid2d.h"
enum SIM_MODE {
    CPU,
    GPU
};

int main() {

    // grid infos
    constexpr float res = 2.f;
    const int width = static_cast<int>(128.f * res);
    const int height = static_cast<int>(72.f * res);
    const int cell_size = static_cast<int>(16.f / res);
    // Fluid infos
    constexpr float diffusion_rate = 0.0001f;
    constexpr float viscosity_rate = 0.00000000001f;
    constexpr int sub_step = 25;
    // simulation infos
    constexpr SIM_MODE sim_mode = CPU;
    constexpr float time_accel = 1.f;
    constexpr DRAW_MODE draw_mode = VELOCITY;
    const int add_radius = 3 * res;
    constexpr float add_intensity = 10.f;


    printf("Parameters set. \n");

    // ----{ Choosing simulation }----
    Fluid* fluid;
    if constexpr (sim_mode == CPU) {
        fluid = new CpuFluid2d(
                width,
                height,
                cell_size,
                diffusion_rate,
                viscosity_rate, sub_step,
                add_radius,
                add_intensity);
    }
    else {
        fluid = new GlFluid2d(width, height, cell_size, diffusion_rate, viscosity_rate, sub_step, add_radius, add_intensity);
    }
    printf("simulation chosed. \n");
    // ----{ Main Loop }----
    fluid->run_loop(draw_mode, time_accel);
    printf("Main loop over. \n");
    // ----{ Clean Up }----
    delete fluid;
    return 0;
}
