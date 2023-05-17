#include <iostream>
#include <GLFW/glfw3.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "gl_textrenderer/gl_textrenderer.h"
#include "gl_gridlines/gl_gridlines.h"

using namespace gl;

const unsigned int SCREEN_WIDTH = 500;
const unsigned int SCREEN_HEIGHT = 500;

// will be used by all 3 objects
const std::string vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;

        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * vec4(pos.xy, 1.0, 1.0);
        }
)";

void create_shader_program();
void draw_rectangle();
void draw_triangle();
void draw_line();

int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "gl_textrenderer", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glbinding::initialize(glfwGetProcAddress);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}

unsigned int create_shader_program(const std::string& vertex_source, const std::string& fragment_source)
{
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* c_str_vertex = vertex_source.c_str();
    glShaderSource(vertexShader, 1, &c_str_vertex, nullptr);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* c_str_fragment = fragment_source.c_str();
    glShaderSource(fragmentShader, 1, &c_str_fragment, nullptr);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void draw_rectangle()
{
    /*
    *   B - C
    *   | / |
    *   A - D
    */
    std::array<int, 8> verticies {
            100, 100,  // A
            100, 200,  // B
            200, 200,  // C
            200, 100   // D
    };

    std::array<int, 6> indicies {
            0, 1, 2,
            0, 2, 3
    };

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(int), verticies.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(verticies), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(int), indicies.data(), GL_STATIC_DRAW);



    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


