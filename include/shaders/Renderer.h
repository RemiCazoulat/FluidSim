//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"

enum R_MODE {
    VEL,
    DENS,
};

class Renderer {
    std::vector<float> vertices;
    std::vector<int> indices;
    GLuint VAO, VBO, EBO;
    GLuint renderProgram;
    void createGeometry();
public:
    Renderer(const char* vertexPath, const char* fragmentPath);
    ~Renderer();
    void rendering(const GLuint &colorTex) const;
};



#endif //RENDERER_H
