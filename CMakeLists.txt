cmake_minimum_required(VERSION 3.28)
project(FluidSim)

set(CMAKE_CXX_STANDARD 17)
# opengl
find_package(OpenGL REQUIRED)
# glfw
add_subdirectory(thirdparty/glfw)
# glm
add_subdirectory(thirdparty/glm)
# glad
add_library(glad "thirdparty/glad/src/glad.c")
target_include_directories(glad PUBLIC thirdparty/glad/include)
# Dear ImGui
set(IMGUI_DIR thirdparty/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

add_executable(
        ${PROJECT_NAME}
        # ----{ Simulation }----
        # mandatory files
        include/libraries.h # libraries
        src/main.cpp # main
        include/shaders/shader.h # shader
        src/shaders/shader.cpp
        include/shaders/compute.h # compute shader
        src/shaders/compute.cpp
        include/shaders/Renderer.h # renderer
        src/shaders/Renderer.cpp
        include/sim/Fluid.h # abstract Fluid class
        src/sim/Fluid.cpp
        include/SimData.h # Struct to pass data between simulation and interface
        src/SimData.cpp
        include/ui/Interface.h # Interface
        src/ui/Interface.cpp
        # 2D files
        include/sim/2D/Fluid2D.h # abstract Fluid2D class
        src/sim/2D/Fluid2D.cpp
        include/sim/2D/GLFlu2D.h # GL Fluid 2D
        src/sim/2D/GLFlu2D.cpp
        include/sim/2D/cpuFlu2D.h # CPU Fluid 2D
        src/sim/2D/cpuFlu2D.cpp
        src/sim/2D/GLoFlu2D.cpp # Optimized GL Fluid 2D (not working for now)
        include/sim/2D/GLoFlu2D.h
        # ----{ thirdparty }----
        thirdparty/miniaudio.h
        thirdparty/stb_image.h
        # Dear ImGui
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        include/SoundConvertor.cpp
        include/SoundConvertor.h


)

target_link_libraries(
        FluidSim
        glfw
        glm::glm
        glad
        OpenGL::GL
)

