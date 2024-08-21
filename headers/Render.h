//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef RENDER_H
#define RENDER_H

#include "Shader.h"





class Render {
    /*
    float vertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 3, 2
    };
    */
    std::vector<float> vertices;
    std::vector<int> indices;

    GLuint VAO, VBO, EBO;

    void createGeometry();
public:
    Render();
    static GLuint createRenderProgram(const char* vertexPath, const char* fragmentPath);
    void makeRender(const GLuint & renderProgram, const GLuint & velTex, const GLuint & densTex) const;
    void cleanRender(const GLuint & renderProgram) const;
};



#endif //RENDER_H
