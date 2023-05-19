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

const std::string vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;

        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * vec4(aPos.xy, 1, 1);
        }
)";

const std::string fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;

        uniform vec3 color;

        void main()
        {
            FragColor = vec4(color.xyz, 1.0f);
        }
)";

unsigned int create_shader_program(const std::string& vertex_source, const std::string& fragment_source);
void draw_rectangle();
void draw_triangle(int triangle_pos_x);
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

    unsigned int shaderProgram = create_shader_program(vertex_shader_source, fragment_shader_source);
    glUseProgram(shaderProgram);
    glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.5f, 0.5f);

    // frame timing
    double delta_time = 0.0f;
    double last_frame = 0.0f;

    // positions
    int triangle_pos_x = SCREEN_WIDTH;

    gl_textrenderer textrenderer(SCREEN_WIDTH, SCREEN_HEIGHT, "assets/UbuntuMono-R.ttf", 13, {1.0f, 1.0f, 1.0f, 1.1f});

    int score = 0;

    srand(1);
    while (!glfwWindowShouldClose(window))
    {
        double current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        std::string score_text = "score: " + std::to_string(score);
        textrenderer.render_text(score_text, 10, SCREEN_HEIGHT - 20);

        glUseProgram(shaderProgram);
        // update positions
        {
            int triangle_reset_pos_x = -((rand() % 50) * 100) - 200;
            if (triangle_pos_x < triangle_reset_pos_x)
            {
                triangle_pos_x = SCREEN_WIDTH;
                // add score each time triangle gets reset
                score += 100;
            }
            triangle_pos_x -= (300 + (score / 1.8)) * delta_time;
        }

        {
            glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.2f, 0.7f);
            draw_rectangle();
        }

        {
            glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.7f, 0.2f, 0.0f);
            draw_triangle(triangle_pos_x);
        }

        {
            glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f);
            draw_line();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
    unsigned int width = 60;
    unsigned int height = 60;
    unsigned int pos_x = 100;
    unsigned int pos_y = 100;
    /*
    *   B - C
    *   | / |
    *   A - D
    */
    std::array<GLuint, 8> vertices {
            pos_x,         pos_y,           // A
            pos_x,         pos_y +height,   // B
            pos_x + width, pos_y + height,  // C
            pos_x + width, pos_y            // D
    };

    std::array<GLuint, 6> indices {
            0, 1, 2,
            0, 2, 3
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLuint), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(GLuint), (const void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_triangle(int triangle_pos_x)
{
    int width = 50;
    int height = 50;
    int pos_x = triangle_pos_x;
    int pos_y = 100;
    /*
    *     B
    *    / \
    *   A - C
    */
    std::array<int, 8> vertices {
            pos_x,                 pos_y,            // A
            pos_x + width / 2,     pos_y + height,   // B
            pos_x + width,         pos_y,            // C
    };

    std::array<GLuint, 6> indices {
            0, 1, 2,
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLuint), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (const void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_line()
{
    /*
     *   A --- B
    */
    std::array<GLuint, 8> vertices {
            0, 100,  // A
            SCREEN_WIDTH, 100,  // B
    };

    std::array<GLuint, 2> indices {
            0, 1,
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLuint), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(GLuint), (const void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}