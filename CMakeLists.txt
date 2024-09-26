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
        # Dear ImGui
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        # Simulation
        src/main.cpp
        include/shaders/shader.h
        include/shaders/compute.h
        src/shaders/compute.cpp
        include/libraries.h
        src/shaders/shader.cpp
        src/sim/2D/GlFluid2D.cpp
        include/sim/2D/GlFluid2D.h
        src/shaders/Renderer.cpp
        include/shaders/Renderer.h
        include/sim/2D/Fluid2D.h
        src/sim/2D/CpuFluid2D.cpp
        include/sim/2D/CpuFluid2D.h
        src/sim/2D/Fluid2D.cpp
        include/sim/Fluid.h
        src/sim/2D/GlFluid2DOpti.cpp
        include/sim/2D/GlFluid2DOpti.h
        src/sim/Fluid.cpp
        #thirdparty
        thirdparty/stb_image.h
        include/sim/SimData.h
)

target_link_libraries(
        FluidSim
        glfw
        glm::glm
        glad
        OpenGL::GL
)

