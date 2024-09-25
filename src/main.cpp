#include "../../include/shaders/Renderer.h"
#include "../include/sim/2D/CpuFluid2D.h"
#include "../include/sim/2D/GlFluid2D.h"
#include "../include/sim/2D/GlFluid2DOpti.h"

enum SIM_MODE {
    CPU,
    GPU
};

enum SIM_DIM {
    TWO_D,
    THREE_D
};

GLFWwindow* initWindow(const int width, const int height, const int cell_size) {
    const int window_width = width * cell_size;
    const int window_height = height * cell_size;
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "OpenGL 2D Fluid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    // Load OpenGL functions using glfwGetProcAddress
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }
    return window;
}


int main() {
    // ----{ Temporary panel }----
    // grid infos
    int resolution = 3;
    auto res_squared = (float)(std::pow(2, resolution));
    const int width = static_cast<int>(128.f * res_squared);
    const int height = static_cast<int>(72.f * res_squared);
    const int cell_size = static_cast<int>(16.f / res_squared);
    // Fluid infos
    float diffusion_rate = 0.0001f;
    float viscosity_rate = 0.00000000001f;
    int sub_step = 25;
    // simulation infos
    constexpr SIM_MODE sim_mode = GPU;
    float time_accel = 1.f;
    constexpr DRAW_MODE draw_mode = VELOCITY;
    float add_radius = 3 * res_squared;
    constexpr float add_intensity_x = 10.f;
    constexpr float add_intensity_y = 0.f;
    const float intensities[]{add_intensity_x, add_intensity_y};
    bool smoke = false;
    bool obstacles = false;
    bool velocity = true;
    float smoke_color[4] = { 1.0f,1.0f,1.0f,1.0f };
    bool smoke_perm = false;
    bool smoke_add = true;
    bool smoke_remove = false;
    float smoke_radius = 1;
    bool obstacles_add = false;
    bool obstacles_remove = false;
    float obstacles_radius = 1;
    float vel_intensity[3] = {0.0f, 0.0f, 0.0f};
    float vel_radius = 1;

    // UI infos
    float zoom = 1.4f;
    const int window_width = width * cell_size;
    const int window_height = height * cell_size;
    ImVec2 left_up_pos = ImVec2(0.0f, 0.0f);
    ImVec2 right_up_pos = ImVec2((float)(window_width), 0.0f);
    ImVec2 left_down_pos = ImVec2(0.0f, (float)(window_height));
    ImVec2 right_down_pos = ImVec2((float)(window_width), (float)(window_height));
    ImVec2 left_up_pivot = ImVec2(0.0f, 0.0f);
    ImVec2 right_up_pivot = ImVec2(1.0f, 0.0f);
    ImVec2 left_down_pivot = ImVec2(0.0f, 1.0f);
    ImVec2 right_down_pivot = ImVec2(1.0f, 1.0f);
    ImVec2 input_pos = left_up_pos;
    ImVec2 input_size = left_up_pos;
    ImVec2 menu_pos = left_up_pos;

    const int values[] = {25, 50, 75, 100, 125, 150, 175, 200, 225, 250};
    const char* valueNames[] = {"25", "50", "75", "100", "125", "150", "175", "200", "225", "250"};

    int selectedValueIndex = 0;



    // ----{ init functions }----
    GLFWwindow* window = initWindow(width, height, cell_size);
    std::cout << glGetString(GL_VERSION) << std::endl;

    // ----{ Choosing simulation }----
    Fluid* fluid;
    if (sim_mode == CPU) {
        fluid = new CpuFluid2D(window, width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    } else {
        fluid = new GlFluid2D(window, width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }

    // ----{ init ImGui }----
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // ----{ Main Loop }----
    int frame_number = 0;
    double total_time = 0.0;
    double previous_time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        // time management
        const auto current_time = glfwGetTime();
        const float dt = (float)(current_time - previous_time) * time_accel;
        previous_time = current_time;

        // ----{ Simulation }----
        glfwPollEvents();
        fluid->input_step(add_radius, intensities, dt);
        fluid->velocity_step(dt);
        fluid->density_step(dt);
        fluid->draw_step(draw_mode);

        // ----{ ImGui }----
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Input Frame
#pragma region input_frame
        ImGui::SetNextWindowPos(left_up_pos, ImGuiCond_Always, left_up_pivot);
        ImGui::Begin("Input");
        ImGui::SetWindowFontScale(zoom);
        ImGui::Text("Input Mode : ");
        if (ImGui::Button("Smoke")) {
            smoke = true;
            obstacles = false;
            velocity = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Obstacles")) {
            smoke = false;
            obstacles = true;
            velocity = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Velocity")) {
            smoke = false;
            obstacles = false;
            velocity = true;
        }
        ImGui::Separator();
        // Smoke
        ImGui::Text("Smoke :");
        ImGui::ColorEdit3("Color", smoke_color);
        ImGui::SliderFloat("Smoke Radius", &smoke_radius, 0, 20);

        if (ImGui::Button("Perm (S)")) {
            smoke_perm = true;
            smoke_add = false;
            smoke_remove = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Add (S)")) {
            smoke_perm = false;
            smoke_add = true;
            smoke_remove = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove (S)")) {
            smoke_perm = false;
            smoke_add = false;
            smoke_remove = true;
        }
        ImGui::Separator();
        // Obstacles
        ImGui::Text("Obstacles :");
        ImGui::SliderFloat("Obstacles Radius", &obstacles_radius, 0, 20);
        if (ImGui::Button("Add (O)")) {
            obstacles_remove = false;
            obstacles_add = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove (O)")) {
            obstacles_remove = true;
            obstacles_add = false;
        }
        ImGui::Separator();
        // Velocity
        ImGui::Text("Velocity :");
        ImGui::SliderFloat("Vel Radius", &vel_radius, 0, 20);
        float x = 0;
        float y = 0;
        float z = 0;
        float w_avail = ImGui::GetContentRegionAvail().x * 0.2f;
        ImGui::PushItemWidth(w_avail);
        ImGui::SliderFloat("x", &x, 0, 20); ImGui::SameLine();
        ImGui::SliderFloat("y", &y, 0, 20); ImGui::SameLine();
        ImGui::SliderFloat("z", &z, 0, 20); ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::Text("Intensity");
        vel_intensity[0] = x; vel_intensity[1] = y; vel_intensity[2] = z;


        //ImGui::SliderFloat3("Intensity", vel_intensity, 0.0, 100.0);
        input_size = ImGui::GetWindowSize();
        ImGui::End();
#pragma endregion
        // Menu Frame
#pragma region menu_frame
        menu_pos = ImVec2(input_pos.x + input_size.x, input_pos.y);
        ImGui::SetNextWindowPos(menu_pos, ImGuiCond_Always);
        //ImGui::Begin("Menu");
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("Menu", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar );
        ImGui::SetWindowFontScale(zoom);

        if (ImGui::Button("Reset")) {
        }
        if (ImGui::Button("Import")) {
        }
        if (ImGui::Button("Export")) {
        }
        if (ImGui::Button("Fullscreen")) {
        }
        if (ImGui::Button("Pause")) {
        }
        ImGui::End();

#pragma endregion
        // Debug Frame
#pragma region debug_frame
#pragma endregion
        // Simulation Frame
#pragma region simulation_frame
        menu_pos = ImVec2(input_pos.x, input_pos.y + input_size.y + 20);
        ImGui::SetNextWindowPos(menu_pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(input_size.x, 0));
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

        ImGui::SliderInt("Resolution", &resolution, 0, 4);

        w_avail = ImGui::GetContentRegionAvail().x * 0.3f;
        ImGui::PushItemWidth(w_avail);
        ImGui::SliderFloat("##time_accel_slider", &time_accel, 0.001f, 100.f); ImGui::SameLine();
        ImGui::InputFloat("##time_accel_input", &time_accel, 0.001f, 100.f); ImGui::SameLine();
        ImGui::Text("Time acceleration");
        ImGui::PopItemWidth();

        ImGui::Separator();
        ImGui::Text("Fluid infos");
        ImGui::InputFloat("Diffusion", &diffusion_rate, 0.1f, 1.0f, "%.11f");
        ImGui::InputFloat("Viscosity", &viscosity_rate, 0.1f, 1.0f, "%.11f");


        if (ImGui::BeginCombo("Sub steps", valueNames[selectedValueIndex])) {
            for (int i = 0; i < IM_ARRAYSIZE(values); i++) {
                bool isSelected = (selectedValueIndex == i);
                if (ImGui::Selectable(valueNames[i], isSelected)) {
                    selectedValueIndex = i; // Met à jour l'index sélectionné
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
        sub_step = values[selectedValueIndex];
        ImGui::Text("Sub steps selected : %d", sub_step);

        ImGui::Separator();
        ImGui::Text("Drawing info");
        if(ImGui::Button("Velocity")) {
        }; ImGui::SameLine();
        if(ImGui::Button("Density")) {
        }; ImGui::SameLine();
        if(ImGui::Button("Pressure")) {
        }
        ImGui::End();
#pragma endregion

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // ----{ rendering }----
        // ----{ Swap Buffers }----
        glfwSwapBuffers(window);
        // time management
        total_time += glfwGetTime() - current_time;
        frame_number++;
    }

    // ----{ Debug }----
    fluid->debug();
    printf("\n");
    printf("=========[ Main Debug ]=========\n");
    printf("total time: %f s\n", total_time);
    printf("\n");
    printf("total time per frame: %f ms (%.2f FPS)\n", total_time / frame_number * 1000.0, 1.f / (total_time / frame_number));

    // ----{ Clean Up }----
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    delete fluid;
    return 0;
}
