//
// Created by Rémi on 27/09/2024.
//

#ifndef FLUIDSIM_INTERFACE_H
#define FLUIDSIM_INTERFACE_H
#include "../libraries.h"
#include "FluidSim/include/SimData.h"
#include "FluidSim/include/sim/Fluid.h"


class Interface {
    SimData* simData;

    float zoom = 1.4f;
    ImVec2 left_up_pos;
    ImVec2 right_up_pos;
    ImVec2 left_down_pos;
    ImVec2 right_down_pos;
    ImVec2 left_up_pivot = ImVec2(0.0f, 0.0f);
    ImVec2 right_up_pivot = ImVec2(1.0f, 0.0f);
    ImVec2 left_down_pivot = ImVec2(0.0f, 1.0f);
    ImVec2 right_down_pivot = ImVec2(1.0f, 1.0f);
    ImVec2 input_pos = left_up_pos;
    ImVec2 input_size = left_up_pos;
    ImVec2 menu_pos = left_up_pos;
    // sub step combobox variables
    const int sub_step_values[11] = {1,25, 50, 75, 100, 125, 150, 175, 200, 225, 250};
    const char* sub_step_names[11] = {"1","25", "50", "75", "100", "125", "150", "175", "200", "225", "250"};
    int sub_step_index = 1;
    // textures for images
    GLuint color_wheel_tex;
    // verifying variables
    bool is_init = false;
    // min and max variables
    int max_res = 4;
public:
    explicit Interface(GLFWwindow* window, SimData* simData);
    ~Interface();
    void initFrame();
    void runInputWindow();
    void runSimulationWindow();
    void runMenuWindow();
    void runDebugWindow(float dt, int &frame_number, double &total_time);
    void runColorWheelWindow();
    void renderFrame();
};


#endif //FLUIDSIM_INTERFACE_H
