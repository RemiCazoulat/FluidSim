//
// Created by remi.cazoulat on 28/06/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "../libraries.h"

std::string readShaderCode(const char* filePath);
GLuint compileShader(const char* shaderCode, GLenum shaderType);
void createUniform1f(const GLuint & program, const GLchar* name, const float & value);
void bindingUniformTex(const GLuint & program, const GLchar * name, const int & bindIndex);
GLuint createTextureVec1(const GLfloat * data, int width, int height);
GLuint createTextureVec2(const GLfloat * data, int width, int height);
GLuint createTextureVec4(const GLfloat * data, int width, int height);

#endif //SHADER_H
