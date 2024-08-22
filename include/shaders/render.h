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

class Render {
    std::vector<float> vertices;
    std::vector<int> indices;

    GLuint VAO, VBO, EBO;

    void createGeometry();
public:
    Render();
    void makeRender(const GLuint & renderProgram, const GLuint & velTex, const GLuint & densTex, const R_MODE renderingMode) const;
    void cleanRender(const GLuint & renderProgram) const;
};

GLuint createRenderProgram(const char* vertexPath, const char* fragmentPath);


#endif //RENDER_H
