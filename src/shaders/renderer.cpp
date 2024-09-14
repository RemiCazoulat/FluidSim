//
// Created by remi.cazoulat on 20/08/2024.
//

#include "../../include/shaders/Renderer.h"

void Renderer::createGeometry() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(int)), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        static_cast<void *>(nullptr)
         );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        reinterpret_cast<void *>(3 * sizeof(float))
        );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Renderer::Renderer(const char* vertexPath, const char* fragmentPath) {
    vertices = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };
    indices = {
        0, 1, 2,
        0, 3, 2
    };
    VAO = 0;
    VBO = 0;
    EBO = 0;
    createGeometry();

    const std::string vertexCode = readShaderCode(vertexPath);
    const std::string fragmentCode = readShaderCode(fragmentPath);
    const GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    const GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);
    const GLuint renderProgram = glCreateProgram();
    glAttachShader(renderProgram, vertexShader);
    glAttachShader(renderProgram, fragmentShader);
    glLinkProgram(renderProgram);
    GLint success;
    glGetProgramiv(renderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(renderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    this->renderProgram = renderProgram;
    bindingUniformTex(renderProgram, "colorTex", 0);
}

Renderer::~Renderer() {
    glDeleteProgram(renderProgram);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Renderer::rendering(const GLuint &colorTex) const {
    glUseProgram(renderProgram);
    // Activer et lier les textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    // Rendu
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

}

