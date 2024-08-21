#include "headers/Render.h"
#include "headers/Compute.h"


GLFWwindow* window;
double mouseX, mouseY;
bool leftButtonPressed = false;
bool rightButtonPressed = false;

int gridWidth;
int gridHeight;
int pixelPerCell;

void mouse_button_callback(const int button,const int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        leftButtonPressed = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        rightButtonPressed = (action == GLFW_PRESS);
    }
}

void cursor_position_callback(const  double xpos,const double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}

glm::ivec2 screenToTextureCoordinates(double x, double y) {
    const int windowWidth = gridWidth * pixelPerCell;
    const int windowHeight = gridHeight * pixelPerCell;
    const int texX = static_cast<int>((x / windowWidth) * gridWidth);
    const int texY = static_cast<int>((1.0 - (y / windowHeight)) * gridHeight);
    return {texX, texY};
}

void addVelocity(const GLuint computeShader, const double x,const double y) {
    // Convertir les coordonnées de la souris en coordonnées de texture
    const glm::vec2 texCoord = screenToTextureCoordinates(x, y);

    // Envoyer les nouvelles données au compute shader
    glUseProgram(computeShader);
    glUniform2f(glGetUniformLocation(computeShader, "mousePos"), texCoord.x, texCoord.y);
    glUniform1i(glGetUniformLocation(computeShader, "addVelocity"), 1);

    // Lancez le compute shader
    glUseProgram(0);
}

void addDensity(const GLuint computeShader, const double x,const double y) {
    // Convertir les coordonnées de la souris en coordonnées de texture
    const glm::vec2 texCoord = screenToTextureCoordinates(x, y);

    // Envoyer les nouvelles données au compute shader
    glUseProgram(computeShader);
    glUniform2f(glGetUniformLocation(computeShader, "mousePos"), texCoord.x, texCoord.y);
    glUniform1i(glGetUniformLocation(computeShader, "addDensity"), 1);

    // Lancez le compute shader
    glUseProgram(0);
}

// Init Window, GLFW and GLAD
void initWindow(const int & windowWidth, const int & windowHeight) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create variables

    // Create a GLFW window
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL 2D Fluid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    // Load OpenGL functions using glfwGetProcAddress
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }
}

GLuint createTextureVec2(const GLfloat * data, const int width, const int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, data);
    return texture;
}
GLuint createTextureVec1(const GLfloat * data, const int width, const int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}
// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------
// -----------{ Main function }--------------------------
// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------
// ---{ M }---
// ---{ A }---
// ---{ I }---
// ---{ N }---
int main() {
    // Init grid
    gridWidth = 128 /2;
    gridHeight = 72 /2;
    pixelPerCell = 32;
    const int gridSize = gridWidth * gridHeight;
    const int gridSizex2 = gridSize * 2;
    auto* vel = new GLfloat[gridSizex2]();
    auto* density = new GLfloat[gridSize]();
    auto* densityTransi = new GLfloat[gridSize]();

    printf("[DEBUG] init arrays \n");

    auto circleCoord = glm::vec2(128 / 2, 72 / 8);
    float radius = 20.0;
    for(int j = 0; j < gridHeight ; j ++) {
        for(int i = 0; i < gridWidth; i ++) {
            const float distance = glm::distance(glm::vec2(i, j), circleCoord);
            density[(i + j * gridWidth)] = distance < radius ? 1.0 : 0.2;

            if( j > gridHeight / 2 - 10 && j < gridHeight / 2 + 10) {
                vel[(i + j * gridWidth) * 2] = 1.0;
                vel[(i + j * gridWidth) * 2 + 1] = 0.0;
            } else {
                vel[(i + j * gridWidth) * 2] = 0.0;
                vel[(i + j * gridWidth) * 2 + 1] = 0.0;
            }
        }
    }

    printf("[DEBUG] init arrays values \n");


    // ---------- { Init Window }----------
    const int windowWidth = pixelPerCell * gridWidth;
    const int windowHeight = pixelPerCell * gridHeight;
    printf("[DEBUG] init window size :  %i %i", windowWidth, windowHeight);
    initWindow(windowWidth, windowHeight);
    printf("[DEBUG] init window done \n");

    // ---------- { Init Textures }----------
    const GLuint velTex = createTextureVec2(vel, gridWidth, gridHeight);
    const GLuint densTex = createTextureVec1(density, gridWidth, gridHeight);
    const GLuint densTexTransit = createTextureVec1(densityTransi, gridWidth, gridHeight);

    printf("[DEBUG] init textures done \n");

    // ---------- { Compute program }----------
    const GLuint computePhysics = createComputeProgram("../glsl/cPhysics.glsl");
    const GLuint computeReplace = createComputeProgram("../glsl/cReplace.glsl");

    printf("[DEBUG] init compute done \n");
    //Compute shader that will calculate the new density and velocity using the old values
    glUseProgram(computePhysics);
    glBindImageTexture (0, velTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    glBindImageTexture (1, densTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (2, densTexTransit, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    // Compute shader that will swap the old and new values while reinit the old values to 0
    glUseProgram(computeReplace);
    glBindImageTexture (0, densTexTransit, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture (1, densTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);


    // ---------- { Shader program }----------
    /*
    createGeometry();
    const GLuint shaderProgram = createRenderProgram("../glsl/vShader.glsl","../glsl/fShader.glsl");
    glUseProgram(shaderProgram);
    bindingUniformTex(shaderProgram, "velTex", 0);
    bindingUniformTex(shaderProgram, "densTex", 1);
    printf("[DEBUG] init shader done \n");
    */
    const Render render;
    const GLuint renderProgram = Render::createRenderProgram("../glsl/vShader.glsl","../glsl/fShader.glsl");
    bindingUniformTex(renderProgram, "velTex", 0);
    bindingUniformTex(renderProgram, "densTex", 1);
    printf("[DEBUG] init shader done \n");



    // ---------- { Main render loop }----------
    while (!glfwWindowShouldClose(window)) {
        execute(computePhysics, gridWidth, gridHeight);
        execute(computeReplace, gridWidth, gridHeight);
        render.makeRender(renderProgram, velTex, densTex);

        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Clean up
    render.cleanRender(renderProgram);
    cleanCompute(computePhysics);
    glfwDestroyWindow(window);
    glfwTerminate();

    delete[] vel;
    delete[] density;

    return 0;
}
