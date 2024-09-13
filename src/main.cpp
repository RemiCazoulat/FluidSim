#include "../../include/shaders/renderer.h"
#include "../include/sim/obstacleFlu.h"
#include "../../include/sim/fluGpu.h"
enum SIM_MODE {
    CPU,
    GPU
};

int width;
int height;
int cell_size;

int main() {
// /////////// Control Panel ////////
/**/// grid infos
/**/constexpr float res = 4.f;
/**/width = static_cast<int>(128.f * res);
/**/height = static_cast<int>(72.f * res);
/**/cell_size = static_cast<int>(16.f / res);
/**/// fluid infos
/**/constexpr float diffusion_rate = 0.0001f;
/**/constexpr float viscosity_rate = 0.00000000001f;
/**/constexpr int sub_step = 25;
/**/// simulation infos
/**/constexpr SIM_MODE sim_mode = GPU;
/**/constexpr float time_accel = 1.f;
/**/constexpr DRAW_MODE draw_mode = VELOCITY;
/**/const int add_radius = 3 * res;
/**/constexpr float add_intensity = 10.f;
// /////////// End of control Panel ////////


    // ////// Choosing simulation ///////
    fluid* fluid;
    if constexpr (sim_mode == CPU) {
        //fluid = new obstacleFlu( width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }
    else {
        fluid = new fluGpu(width, height, cell_size, diffusion_rate, viscosity_rate, sub_step, add_radius, add_intensity);
    }
    // ////// Main loop ///////
    fluid->run_loop(draw_mode, time_accel);

    // /////// Clean up ///////
    delete fluid;
    return 0;
}
