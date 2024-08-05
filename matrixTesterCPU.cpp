#include "shader.h"
#include "compute.h"
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

void normalizeFloat(const int line, const int column, const int matrixSize, const float ii, std::vector<GLfloat> & matrix, std::vector<GLfloat> & matrixID) {
    if (line < column) return;
    if (line > matrixSize + column) return;
    if (line < matrixSize) {
        const float value = matrix[line + column * matrixSize];
        if (value == 0.0) return;
        float newValue = 1.0;
        if(line != column) newValue = value / ii;
        matrix[line + column * matrixSize] = newValue;
        return;
    }
    const int newLine = line - matrixSize;
    const float value = matrixID[newLine + column * matrixSize];
    if (value == 0.0) return;
    const float newValue = value / ii;
    matrixID[newLine + column * matrixSize] = newValue;
}

void subtractLine(const int line,const int lineToSub, const int matrixSize, std::vector<GLfloat> & matrix, std::vector<GLfloat> & matrixID) {
    float mult = matrix[lineToSub + line * matrixSize];
    if(mult == 0.0) return;
    matrix[lineToSub + line * matrixSize] = 0.0;
    ///////////////////////////////////////
    // loop to substract the initial line
    ///////////////////////////////////////
    for(int i = lineToSub + 1; i < matrixSize; i ++) {
        const float value = matrix[i + line * matrixSize];
        matrix[i + line * matrixSize] = value - mult * matrix[i + lineToSub * matrixSize];;
    }
    ///////////////////////////////////////
    // loop to substract the inversed line
    ///////////////////////////////////////
    for(int i = 0; i < lineToSub; i ++) {
        const float value = matrixID[i + line * matrixSize];
        matrixID[i + line * matrixSize] = value - mult * matrixID[i + lineToSub * matrixSize];;
    }
}
int main() {
    constexpr int matrixSize = 3;
    // Matrix ID
    std::vector matrixID(matrixSize * matrixSize, 0.0f);
    for(int i = 0; i < matrixSize; i++) {
        for(int j = 0; j < matrixSize; j++) {
            matrixID[i + j * matrixSize] = i == j ? 1.0f : 0.0f;
        }
    }

    //Data
    std::vector<GLfloat> matrix = {
    3, -2, 4,
    2, -4, 5,
    1,  8, 2
    };


    for(int i = 0; i < matrixSize; i ++) {
        const float ii =  matrix[i + i * matrixSize];
        printf("ii: %f\n", ii);
        for(int j = 0; j < matrixSize * 2; j ++) {
            normalizeFloat(i, j, matrixSize, ii, matrix, matrixID);

        }
        for(int j = 0; j < matrixSize; j ++) {
            if(j != i) {
                //subtractLine(j, i, matrixSize, matrix, matrixID);
            }
        }
    }



    printf("Matrix ID:\n");
    for(int i = 0; i < matrixSize; i++) {
        for(int j = 0; j < matrixSize; j++) {
            printf("%f ", matrixID[i + j * matrixSize]);
        }
        printf("\n");
    }

    printf("Matrix:\n");
    for(int i = 0; i < matrixSize; i++) {
        for(int j = 0; j < matrixSize; j++) {
            printf("%f ", matrix[i + j * matrixSize]);
        }
        printf("\n");
    }







    return 0;
}