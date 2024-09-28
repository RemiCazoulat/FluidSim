//
// Created by Rémi on 27/09/2024.
//

#include "../../include/ui/Interface.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../thirdparty/stb_image.h"

GLuint image2Tex(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (data == nullptr) {
        std::cerr << "Failed to load image!" << std::endl;
        return 0;
    }
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (channels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return texture;
}

Interface::Interface(GLFWwindow* window, SimData* simData) {
    this->simData = simData;
    left_up_pos = ImVec2(0.0f, 0.0f);
    right_up_pos = ImVec2((float)(simData->window_width), 0.0f);
    left_down_pos = ImVec2(0.0f, (float)(simData->window_height));
    right_down_pos = ImVec2((float)(simData->window_width), (float)(simData->window_height));
    // ----{ init ImGui }----
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    // init textures for ImGui
    color_wheel_tex = image2Tex("../resources/images/ui/color_wheel.png");
}

void Interface::initFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    is_init = true;
}

void Interface::runInputWindow() {
    if (!is_init) {
        printf("[ERROR] Frame is not initialized. Call method initFrame() "
               "before calling other methods of the Interface class. \n");
        return;
    }
    ImGui::SetNextWindowPos(left_up_pos, ImGuiCond_Always, left_up_pivot);
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("Input");
    ImGui::SetWindowFontScale(zoom);
    ImGui::Text("Input Mode : ");
    if (ImGui::Button("Smoke")) {
        simData->smoke = true;
        simData->obstacles = false;
        simData->velocity = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Obstacles")) {
        simData->smoke = false;
        simData->obstacles = true;
        simData->velocity = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Velocity")) {
        simData->smoke = false;
        simData->obstacles = false;
        simData->velocity = true;
    }
    ImGui::Separator();
    // Smoke
    ImGui::Text("Smoke :");
    ImGui::ColorEdit3("Color", simData->smoke_color);

    if (ImGui::Button("Perm (S)")) {
        simData->smoke_perm = true;
        simData->smoke_add = false;
        simData->smoke_remove = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Add (S)")) {
        simData->smoke_perm = false;
        simData->smoke_add = true;
        simData->smoke_remove = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove (S)")) {
        simData->smoke_perm = false;
        simData->smoke_add = false;
        simData->smoke_remove = true;
    }
    ImGui::SliderFloat("Smoke Radius", &simData->smoke_radius, 0, 20);

    ImGui::SliderFloat("Intensity", &simData->smoke_intensity, 0, 100);

    ImGui::Separator();
    // Obstacles
    ImGui::Text("Obstacles :");
    ImGui::SliderFloat("Obstacles Radius", &simData->obstacles_radius, 0, 20);
    if (ImGui::Button("Add (O)")) {
        simData->obstacles_remove = false;
        simData->obstacles_add = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove (O)")) {
        simData->obstacles_remove = true;
        simData->obstacles_add = false;
    }
    ImGui::Separator();
    // Velocity
    ImGui::Text("Velocity :");
    if (ImGui::Button("Perm (V)")) {
        simData->vel_perm = true;
        simData->vel_add = false;
        simData->vel_remove = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Add (V)")) {
        simData->vel_perm = false;
        simData->vel_add = true;
        simData->vel_remove = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove (V)")) {
        simData->vel_perm = false;
        simData->vel_add = false;
        simData->vel_remove = true;
    }
    ImGui::SliderFloat("Vel Radius", &simData->vel_radius, 0, 20);
    float x = simData->vel_intensity[0];
    float y = simData->vel_intensity[1];
    float z = simData->vel_intensity[2];
    float w_avail = ImGui::GetContentRegionAvail().x * 0.2f;
    ImGui::PushItemWidth(w_avail);
    ImGui::SliderFloat("x", &x, 0, 20); ImGui::SameLine();
    ImGui::SliderFloat("y", &y, 0, 20); ImGui::SameLine();
    ImGui::SliderFloat("z", &z, 0, 20); ImGui::SameLine();
    ImGui::PopItemWidth();
    ImGui::Text("Intensity");
    simData->vel_intensity[0] = x;
    simData->vel_intensity[1] = y;
    simData->vel_intensity[2] = z;
    input_size = ImGui::GetWindowSize();
    ImGui::End();
}

void Interface::runSimulationWindow() {
    if (!is_init) {
        printf("[ERROR] Frame is not initialized. Call method initFrame() "
               "before calling other methods of the Interface class. \n");
        return;
    }
    menu_pos = ImVec2(input_pos.x, input_pos.y + input_size.y + 20);
    ImGui::SetNextWindowPos(left_down_pos, ImGuiCond_Always, left_down_pivot);
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("Simulation");
    ImGui::SetWindowFontScale(zoom);
    if (ImGui::Button("2D")) {
    }
    ImGui::SameLine();

    if (ImGui::Button("3D")) {
    }
    ImGui::Text("Simulation mode");
    if (ImGui::Button("CPU")) {
    }
    ImGui::SameLine();

    if (ImGui::Button("GL")) {
    }
    ImGui::SameLine();

    if (ImGui::Button("Vk")) {
    }

    ImGui::SliderInt("Resolution", &simData->resolution, 0, 4);

    float w_avail = ImGui::GetContentRegionAvail().x * 0.3f;
    ImGui::PushItemWidth(w_avail);
    ImGui::SliderFloat("##time_accel_slider", &simData->time_accel, 0.001f, 100.f); ImGui::SameLine();
    ImGui::InputFloat("##time_accel_input", &simData->time_accel, 0.001f, 100.f); ImGui::SameLine();
    ImGui::Text("Time acceleration");
    ImGui::PopItemWidth();

    ImGui::Separator();
    ImGui::Text("Fluid infos");
    ImGui::InputFloat("Diffusion", &simData->diffusion, 0.1f, 1.0f, "%.11f");
    ImGui::InputFloat("Viscosity", &simData->viscosity, 0.1f, 1.0f, "%.11f");


    if (ImGui::BeginCombo("Sub steps", sub_step_names[sub_step_index])) {
        for (int i = 0; i < IM_ARRAYSIZE(sub_step_values); i++) {
            bool isSelected = (sub_step_index == i);
            if (ImGui::Selectable(sub_step_names[i], isSelected)) {
                sub_step_index = i; // Met à jour l'index sélectionné
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus(); // Met le focus sur l'élément sélectionné
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();

    if(ImGui::Button("/!\\")) {
    }
    simData->sub_step = sub_step_values[sub_step_index];
    ImGui::Text("Sub steps selected : %d", simData->sub_step);

    ImGui::Separator();
    ImGui::Text("Drawing info");
    if(ImGui::Button("Velocity")) {
    }; ImGui::SameLine();
    if(ImGui::Button("Density")) {
    }; ImGui::SameLine();
    if(ImGui::Button("Pressure")) {
    }
    ImGui::End();
}

void Interface::runMenuWindow() {
    if (!is_init) {
        printf("[ERROR] Frame is not initialized. Call method initFrame() "
               "before calling other methods of the Interface class. \n");
        return;
    }
    menu_pos = ImVec2(input_pos.x + input_size.x, input_pos.y);
    ImGui::SetNextWindowPos(menu_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Menu", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar );
    ImGui::SetWindowFontScale(zoom);

    if (ImGui::Button("Reset")) {
        // TODO: reset action
    }
    if (ImGui::Button("Import")) {
        // TODO: import action
    }
    if (ImGui::Button("Export")) {
        // TODO: export action
    }
    if (ImGui::Button("Fullscreen")) {
        // TODO: fullscreen action
    }
    if (ImGui::Button("Pause")) {
        // TODO: pause action
    }
    ImGui::End();
}

void Interface::runDebugWindow(const float dt, int &frame_number, double &total_time) {
    if (!is_init) {
        printf("[ERROR] Frame is not initialized. Call method initFrame() "
               "before calling other methods of the Interface class. \n");
        return;
    }
    ImGui::SetNextWindowPos(right_up_pos, ImGuiCond_Always, right_up_pivot);
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Debug" , nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(zoom);
    ImGui::Text("%.2f FPS (%.2f ms)", 1.f / dt, dt * 1000.f);
    ImGui::Text("Time : %.5f", total_time * simData->time_accel);
    ImGui::Text("Real time : %.5f", total_time);
    ImGui::Text("Screen : %i", frame_number);
    ImGui::End();
}

void Interface::runColorWheelWindow() {
    if (!is_init) {
        printf("[ERROR] Frame is not initialized. Call method initFrame() "
               "before calling other methods of the Interface class. \n");
        return;
    }
    ImGui::SetNextWindowPos(right_down_pos, ImGuiCond_Always, right_down_pivot);
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Color wheel" , nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar);
    ImGui::Image((void*)(intptr_t)color_wheel_tex, ImVec2(150, 150));
    ImGui::End();
}

void Interface::renderFrame() {
    if (!is_init) {
        printf("[ERROR] Frame is not initialized. Call method initFrame() "
               "before calling other methods of the Interface class, even in a loop. \n");
        return;
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    is_init = false;
}

Interface::~Interface() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

