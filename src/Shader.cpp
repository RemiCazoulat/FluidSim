//
// Created by remi.cazoulat on 28/06/2024.
//
#include "../headers/Shader.h"


std::string readShaderCode(const char* filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}
// Function to compile a shader
GLuint compileShader(const char* shaderCode, const GLenum shaderType) {
    const GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }
    return shader;
}
// Function to link shaders into a program
GLuint createRenderProgram(const char* vertexPath, const char* fragmentPath) {
    const std::string vertexCode = readShaderCode(vertexPath);
    const std::string fragmentCode = readShaderCode(fragmentPath);
    const GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    const GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);
    const GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

void createUniform1f(const GLuint & shaderProgram, const GLchar* name, const float & value) {
    glUseProgram(shaderProgram);
    const GLint location = glGetUniformLocation(shaderProgram, name);
    glUniform1f(location, value);
    glUseProgram(0);
}

void bindingUniformTex(const GLuint & shaderProgram, const GLchar * name, const int & bindIndex) {
    glUseProgram(shaderProgram);
    const GLint texLoc = glGetUniformLocation(shaderProgram, name);
    glUniform1i(texLoc, bindIndex);
    glUseProgram(0);
}






