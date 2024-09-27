//
// Created by Rémi on 27/09/2024.
//

#ifndef FLUIDSIM_INTERFACE_H
#define FLUIDSIM_INTERFACE_H
#include "../libraries.h"
#include "FluidSim/include/SimData.h"


class Interface {
    SimData* simData = nullptr;

    float zoom = 1.4f;
    ImVec2 left_up_pos = ImVec2(0.0f, 0.0f);
    ImVec2 right_up_pos = ImVec2((float)(simData->window_width), 0.0f);
    ImVec2 left_down_pos = ImVec2(0.0f, (float)(simData->window_height));
    ImVec2 right_down_pos = ImVec2((float)(simData->window_width), (float)(simData->window_height));
    ImVec2 left_up_pivot = ImVec2(0.0f, 0.0f);
    ImVec2 right_up_pivot = ImVec2(1.0f, 0.0f);
    ImVec2 left_down_pivot = ImVec2(0.0f, 1.0f);
    ImVec2 right_down_pivot = ImVec2(1.0f, 1.0f);
    ImVec2 input_pos = left_up_pos;
    ImVec2 input_size = left_up_pos;
    ImVec2 menu_pos = left_up_pos;
    // sub step combobox variables
    const int sub_step_values[10] = {25, 50, 75, 100, 125, 150, 175, 200, 225, 250};
    const char* sub_step_names[10] = {"25", "50", "75", "100", "125", "150", "175", "200", "225", "250"};
    int sub_step_index = 0;
    // textures for images
    GLuint color_wheel_tex;
    // verifying variables
    bool is_init = false;
public:
    explicit Interface(GLFWwindow* window, SimData* simData);
    void initFrame();
    void runInputWindow();
    void runSimulationWindow();
    void runMenuWindow();
    void runDebugWindow(float dt, int &frame_number, float &total_time);
    void runColorWheelWindow();
    void renderFrame();
};


#endif //FLUIDSIM_INTERFACE_H