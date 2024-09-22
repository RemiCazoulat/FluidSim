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
    const int add_radius = 3 * res;
    constexpr float add_intensity_x = 10.f;
    constexpr float add_intensity_y = 0.f;
    const float intensities[]{add_intensity_x, add_intensity_y};

    // ----{ init functions }----
    GLFWwindow* window = initWindow(width, height, cell_size);
    std::cout << glGetString(GL_VERSION) << std::endl;
    const Renderer* renderer = new Renderer("../shaders/vert2d.glsl", "../shaders/frag2d.glsl");
    // ----{ Choosing simulation }----
    Fluid* fluid;
    if (sim_mode == CPU)
    {
        fluid = new CpuFluid2D(window, width, height, cell_size, diffusion_rate, viscosity_rate, sub_step);
    }
    else
    {
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
        GLuint color_texture = fluid->draw_step(draw_mode);
        // ----{ ImGui }----
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderer->rendering(color_texture);

        ImGui::Begin("Smoker Engine");
        ImGui::Text("Create your fluid simulation");
        ImGui::End();

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
    delete renderer;
    delete fluid;
    return 0;
}
