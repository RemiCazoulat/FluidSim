//
// Created by remi.cazoulat on 28/06/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "includes.h"
std::string readShaderCode(const char* filePath);
GLuint compileShader(const char* shaderCode, GLenum shaderType);
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);
void createGeometry();
void createUniform1f(const GLuint & shaderProgram, const GLchar* name, const float & value);
void bindingUniformTex(const GLuint & shaderProgram, const GLchar * name, const int & bindIndex);
void render(const GLuint & shaderProgram, const GLuint & velTex, const GLuint & densTex);
void cleanShader(const GLuint & shaderProgram);

#endif //SHADER_H
