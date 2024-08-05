//
// Created by remi.cazoulat on 28/06/2024.
//
#include "compute.h"
#include "shader.h"

void printWorkGroupsCapabilities() {
    int workgroup_count[3];
    int workgroup_size[3];
    int workgroup_invocations;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);
    printf ("Taille maximale des workgroups:\n\tx:%u\n\ty:%u\n\tz:%u\n",
    workgroup_size[0], workgroup_size[1], workgroup_size[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workgroup_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workgroup_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workgroup_size[2]);
    printf ("Nombre maximal d'invocation locale:\n\tx:%u\n\ty:%u\n\tz:%u\n",
    workgroup_size[0], workgroup_size[1], workgroup_size[2]);
    glGetIntegerv (GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroup_invocations);
    printf ("Nombre maximum d'invocation de workgroups:\n\t%u\n", workgroup_invocations);
}
// Function to compile a compute shader
GLuint createComputeProgram(const char* computePath) {
    printWorkGroupsCapabilities();
    const std::string computeCode = readShaderCode(computePath);
    const GLuint computeShader = compileShader(computeCode.c_str(), GL_COMPUTE_SHADER);
    const GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }
    glDeleteShader(computeShader);
    return program;
}
// Function to execute a compute shader
void execute(const GLuint & program, const GLuint & densTex, const GLuint & densTexTransit, const int & width, const int & height) {
    glUseProgram(program);
    glBindImageTexture (1, densTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, densTexTransit, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    for(int i = 0; i < 10; i ++) {
        glDispatchCompute(width / 64,height / 1,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    /*
    glCopyImageSubData(
        densTexTransit,GL_TEXTURE_2D,0, 0, 0, 0,
        densTex, GL_TEXTURE_2D, 0, 0, 0, 0,
        width, height, 1);
    */
}

void cleanCompute(const GLuint & computeShader) {
    glDeleteProgram(computeShader);
}