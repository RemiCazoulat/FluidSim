//
// Created by remi.cazoulat on 28/06/2024.
//

#ifndef COMPUTE_H
#define COMPUTE_H

#include "Shader.h"

void printWorkGroupsCapabilities();
GLuint createComputeProgram(const char* computePath);
void execute(const GLuint & program, const int & width, const int & height);
void cleanCompute(const GLuint & computeShader);


#endif //COMPUTE_H
