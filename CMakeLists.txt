cmake_minimum_required(VERSION 3.28)
project(FluidSim)

set(CMAKE_CXX_STANDARD 17)

add_executable(FluidSim
        src/main.cpp
        include/shaders/shader.h
        include/shaders/compute.h
        src/shaders/compute.cpp
        include/libraries.h
        src/shaders/shader.cpp
        src/sim/2D/GlFluid2d.cpp
        include/sim/2D/GlFluid2d.h
        src/shaders/renderer.cpp
        include/shaders/Renderer.h
        include/sim/2D/Fluid2d.h
        src/sim/2D/CpuFluid2d.cpp
        include/sim/2D/CpuFluid2d.h
        src/sim/2D/Fluid2d.cpp
        include/sim/Fluid.h
)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)

add_library(glad "thirdparty/glad/src/glad.c")
target_include_directories(glad PUBLIC thirdparty/glad/include)


find_package(OpenGL REQUIRED)

target_link_libraries(FluidSim glfw glm::glm glad OpenGL::GL)

