//
// Created by remi.cazoulat on 20/08/2024.
//

#ifndef RENDER_H
#define RENDER_H

#include "shader.h"

enum R_MODE {
    VEL,
    DENS,
};

class renderer {
    std::vector<float> vertices;
    std::vector<int> indices;
    GLuint VAO, VBO, EBO;
    GLuint renderProgram;
    void createGeometry();
public:
    renderer(const char* vertexPath, const char* fragmentPath);
    ~renderer();
    void rendering(const GLuint &colorTex) const;
};



#endif //RENDER_H
