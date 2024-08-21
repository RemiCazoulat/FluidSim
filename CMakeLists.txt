cmake_minimum_required(VERSION 3.28)
project(FluidSim)

set(CMAKE_CXX_STANDARD 17)

add_executable(FluidSim
        main.cpp
        headers/shader.h
        headers/compute.h
        src/compute.cpp
        headers/includes.h
        src/shader.cpp
        src/Fluid2D.cpp
        headers/Fluid2D.h
        src/Render.cpp
        headers/Render.h
)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)

add_library(glad "thirdparty/glad/src/glad.c")
target_include_directories(glad PUBLIC thirdparty/glad/include)


find_package(OpenGL REQUIRED)

target_link_libraries(FluidSim glfw glm::glm glad OpenGL::GL)

