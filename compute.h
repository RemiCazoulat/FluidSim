//
// Created by remi.cazoulat on 28/06/2024.
//

#ifndef COMPUTE_H
#define COMPUTE_H

#include "includes.h"

void printWorkGroupsCapabilities();
GLuint createComputeProgram(const char* computePath);
void execute(const GLuint & program, const GLuint & densTex, const GLuint & densTexTransit, const int & width, const int & height);
void cleanCompute(const GLuint & computeShader);


#endif //COMPUTE_H
