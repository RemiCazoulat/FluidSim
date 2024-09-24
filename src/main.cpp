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
    float add_radius = 3 * res;
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
        // Input Frame
#pragma region input_frame
        ImGui::NewFrame();
        ImGui::Begin("Input");
        ImGui::SetWindowFontScale(1.2f);
        if (ImGui::Checkbox("Smoke mode", &smoke)) {
            smoke = true;
            obstacles = false;
            velocity = false;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Obstacles mode", &obstacles)) {
            smoke = false;
            obstacles = true;
            velocity = false;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Velocity mode", &velocity)) {
            smoke = false;
            obstacles = false;
            velocity = true;
        }
        ImGui::Separator();
        // Smoke
        ImGui::Text("Smoke :");
        ImGui::ColorEdit3("Smoke color", smoke_color);
        ImGui::SliderFloat("Smoke radius", &smoke_radius, 0, 20);

        if (ImGui::Checkbox("Perm smoke", &smoke_perm)) {
            smoke_perm = true;
            smoke_add = false;
            smoke_remove = false;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Add smoke", &smoke_add)) {
            smoke_perm = false;
            smoke_add = true;
            smoke_remove = false;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Remove smoke", &smoke_remove)) {
            smoke_perm = false;
            smoke_add = false;
            smoke_remove = true;
        }
        ImGui::Separator();
        // Obstacles
        ImGui::Text("Obstacles :");
        ImGui::SliderFloat("Obstacles radius", &obstacles_radius, 0, 20);
        if (ImGui::Checkbox("Add obstacles", &obstacles_add)) {
            obstacles_remove = false;
            obstacles_add = true;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Remove obstacles", &obstacles_remove)) {
            obstacles_remove = true;
            obstacles_add = false;
        }
        ImGui::Separator();
        // Velocity
        ImGui::Text("Velocity :");
        ImGui::SliderFloat("Vel radius", &vel_radius, 0, 20);
        ImGui::SliderFloat3("Intensity", vel_intensity, 0.0, 100.0);
        ImGui::End();
#pragma endregion
        // Menu Frame
#pragma region menu_frame
        ImGui::NewFrame();
        ImGui::Begin("Menu");
        ImGui::End();

#pragma endregion
        // Debug Frame
#pragma region debug_frame
#pragma endregion
        // Simulation Frame
#pragma region simulation_frame
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
