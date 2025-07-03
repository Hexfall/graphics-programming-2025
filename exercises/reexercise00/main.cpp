#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include <iostream>
#include <array>
#include <cmath>

#define PI 3.1416f
#define SIDES 16
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT SCREEN_WIDTH

int buildShaderProgram();
void processInput(GLFWwindow* window);

const float length = .5f * std::sqrt(2.0f);

int main() {
    DeviceGL deviceGl;
    
    Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL");
    if (!window.IsValid()) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        return -1;
    }
    
    deviceGl.SetCurrentWindow(window);
    if (!deviceGl.IsReady()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    int shaderProgram = buildShaderProgram();
    
    std::array<float, 3*(SIDES+1)> vertices;
    std::array<unsigned int, 3*SIDES> indices;

    // first vertex centered at (0, 0, 0)
    vertices[0] = vertices[1] = vertices[2] = 0.0f;
    
    float deltaAngle = 2 * PI / SIDES;
    for (int i = 0; i < SIDES; ++i) {
        float angle = i * deltaAngle;
        vertices[3*i+3] = std::sin(angle)*length;
        vertices[3*i+4] = std::cos(angle)*length;
        vertices[3*i+5] = 0.0f;
        
        indices[3*i] = 0;
        indices[3*i+1] = i + 1;
        indices[3*i+2] = i + 2;
    }
    
    indices[3*SIDES - 1] = 1;
    
    VertexBufferObject  vbo;
    VertexArrayObject   vao;
    ElementBufferObject ebo;
    
    vao.Bind();
    
    vbo.Bind();
    vbo.AllocateData<float>(std::span(vertices));
    
    ebo.Bind();
    ebo.AllocateData<unsigned int>(std::span(indices));
    
    VertexAttribute position(Data::Type::Float, 3);
    vao.SetAttribute(0, position, 0);
    
    VertexBufferObject::Unbind();
    VertexArrayObject::Unbind();
    ElementBufferObject::Unbind();
    
    while (!window.ShouldClose()) {
        processInput(window.GetInternalWindow());
        
        deviceGl.Clear(Color(.2f, .3f, .3f, 1.0f));
        
        glUseProgram(shaderProgram);
        vao.Bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        
        window.SwapBuffers();
        deviceGl.PollEvents();
    }
}
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int buildShaderProgram() {
    const char* vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n\0";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}