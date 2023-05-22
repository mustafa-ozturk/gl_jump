#include <iostream>
#include <GLFW/glfw3.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>

#include "freetype-2.12.0/include/ft2build.h"
#include FT_FREETYPE_H"freetype-2.12.0/include/freetype/freetype.h"

#include "gl_textrenderer/gl_textrenderer.h"
#include "gl_gridlines/gl_gridlines.h"
#include "Shader/Shader.h"

using namespace gl;

const unsigned int SCREEN_WIDTH = 500;
const unsigned int SCREEN_HEIGHT = 500;

void draw_rectangle(int rectangle_width, int rectangle_height, int rectangle_pos_x, int rectangle_pos_y);

void draw_triangle(int triangle_width, int triangle_height, int triangle_pos_x, int triangle_pos_y);

void draw_line();

bool check_collision_x(int rectangle_front, int rectangle_back, int triangle_front, int triangle_back);

bool check_collision_y(int rectangle_bottom);

bool is_space_key_pressed(GLFWwindow* window);

enum GAME_STATE
{
    START, GAME, END
};

int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "gl_jump", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glbinding::initialize(glfwGetProcAddress);

    Shader shader;
    unsigned int shaderProgram = shader.get_shader_program();
    glUseProgram(shaderProgram);
    glm::mat4 projection = glm::ortho(0.0f, (float) SCREEN_WIDTH, 0.0f, (float) SCREEN_HEIGHT);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.5f, 0.5f);

    // frame timing
    double delta_time = 0.0f;
    double last_frame = 0.0f;

    int triangle_width = 50;
    int triangle_height = 50;
    int triangle_pos_x = SCREEN_WIDTH;
    int triangle_pos_y = 100;

    int bg_triangle_pos_x = triangle_pos_x + 550;
    int bg_triangle_pos_y = triangle_pos_y;
    int bg_triangle_height = rand() % 8 * 100 + 200;
    int bg_triangle_width = rand() % 8 * 100 + 200;

    int rectangle_width = 60;
    int rectangle_height = 60;
    int rectangle_pos_x = 100;
    int rectangle_pos_y = 100;

    // jump state
    bool jump = false;
    int jump_amount = 10;

    gl_textrenderer textrenderer(SCREEN_WIDTH, SCREEN_HEIGHT, "assets/UbuntuMono-R.ttf", 13, {1.0f, 1.0f, 1.0f, 1.1f});

    int score = 0;

    int current_game_state = GAME_STATE::START;

    double death_time = 0;

    srand(1);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        double current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        std::string score_text = "score: " + std::to_string(score);
        auto title_text_size = textrenderer.get_text_size("gl_jump");
        auto start_text_size = textrenderer.get_text_size("press [ space ] to start");
        auto score_text_size = textrenderer.get_text_size(score_text);

        switch (current_game_state)
        {
            case GAME_STATE::START:
                // update
                // add small delay if player just died so they
                // don't immediately jump again and start the game
                if (!jump && glfwGetTime() - death_time > 0.2)
                    jump = is_space_key_pressed(window);
                if (jump)
                {
                    rectangle_pos_y += jump_amount;
                    if (rectangle_pos_y > 250)
                    {
                        jump_amount = -10;
                    }
                    if (rectangle_pos_y <= 100)
                    {
                        jump_amount = 10;
                        jump = false;
                        current_game_state = GAME_STATE::GAME;
                        score = 0;
                    }
                }

                // draw
                glUseProgram(shaderProgram);
                glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.2f, 0.7f);
                draw_rectangle(rectangle_width, rectangle_height, rectangle_pos_x, rectangle_pos_y);
                glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f);
                draw_line();

                textrenderer.render_text("gl_jump",
                                         SCREEN_WIDTH / 2 - (title_text_size.first / 2),
                                         (SCREEN_HEIGHT - SCREEN_HEIGHT / 3 ) - (title_text_size.second / 2) + 2
                );
                textrenderer.render_text("press [ space ] to start",
                                         SCREEN_WIDTH / 2 - (start_text_size.first / 2),
                                         (SCREEN_HEIGHT - SCREEN_HEIGHT / 2.6 ) - (start_text_size.second / 2) + 2
                );
                if (score > 0)
                {
                    textrenderer.render_text(score_text,
                                             SCREEN_WIDTH / 2 - (score_text_size.first / 2),
                                             (SCREEN_HEIGHT - SCREEN_HEIGHT / 2.4 ) - (score_text_size.second / 2) + 2
                    );
                }
                break;
            case GAME_STATE::GAME:
                // update
                {
                    score += 1;
                    // rectangle jump
                    if (!jump)
                        jump = is_space_key_pressed(window);
                    if (jump)
                    {
                        rectangle_pos_y += jump_amount;
                        if (rectangle_pos_y > 250)
                        {
                            jump_amount = -10;
                        }
                        if (rectangle_pos_y <= 100)
                        {
                            jump_amount = 10;
                            jump = false;
                        }
                    }
                    // update triangle positions
                    {
                        triangle_pos_x -= (300 + score) * delta_time;
                        bg_triangle_pos_x -= (300 + score) * delta_time;
                        int triangle_reset_pos_x = -((rand() % 50) * 100) - 200;
                        if (triangle_pos_x < triangle_reset_pos_x)
                        {
                            triangle_pos_x = SCREEN_WIDTH;
                            // add score each time triangle gets reset
                        }
                        if (bg_triangle_pos_x <  -(bg_triangle_width * 3))
                        {
                            bg_triangle_pos_x = SCREEN_WIDTH;
                            bg_triangle_height = rand() % 5 * 100 + 200;
                            bg_triangle_width = rand() % 8 * 100 + 200;
                        }
                    }
                    if (check_collision_x(rectangle_pos_x + rectangle_width,
                                          rectangle_pos_x,
                                          triangle_pos_x,
                                          triangle_pos_x + triangle_width) && check_collision_y(rectangle_pos_y))
                    {
                        jump = false;
                        bg_triangle_pos_x = SCREEN_WIDTH + 550;
                        triangle_pos_x = SCREEN_WIDTH;
                        current_game_state = GAME_STATE::START;
                        death_time = glfwGetTime();
                    }
                }
                // draw
                {
                    glUseProgram(shaderProgram);
                    // background triangle
                    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.13f, 0.13f, 0.13f);
                    draw_triangle(bg_triangle_width, bg_triangle_height, bg_triangle_pos_x, bg_triangle_pos_y);

                    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.2f, 0.7f);
                    draw_rectangle(rectangle_width, rectangle_height, rectangle_pos_x, rectangle_pos_y);
                    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.7f, 0.2f, 0.0f);
                    draw_triangle(triangle_width, triangle_height, triangle_pos_x, triangle_pos_y);
                    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f);
                    draw_line();

                    textrenderer.render_text(score_text, 10, SCREEN_HEIGHT - 20);
                }
                break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void draw_rectangle(int rectangle_width, int rectangle_height, int rectangle_pos_x, int rectangle_pos_y)
{
    /*
    *   B - C
    *   | / |
    *   A - D
    */
    std::array<int, 8> vertices{
            rectangle_pos_x, rectangle_pos_y,                       // A
            rectangle_pos_x, rectangle_pos_y + rectangle_height,    // B
            rectangle_pos_x + rectangle_width, rectangle_pos_y + rectangle_height,    // C
            rectangle_pos_x + rectangle_width, rectangle_pos_y                        // D
    };

    std::array<GLuint, 6> indices{
            0, 1, 2,
            0, 2, 3
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(int), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (const void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_triangle(int triangle_width, int triangle_height, int triangle_pos_x, int triangle_pos_y)
{
    /*
    *     B
    *    / \
    *   A - C
    */
    std::array<int, 8> vertices{
            triangle_pos_x, triangle_pos_y,                     // A
            triangle_pos_x + triangle_width / 2, triangle_pos_y + triangle_height,   // B
            triangle_pos_x + triangle_width, triangle_pos_y,                     // C
    };

    std::array<GLuint, 6> indices{
            0, 1, 2,
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLuint), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (const void*) 0);
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
    std::array<GLuint, 8> vertices{
            0, 100,  // A
            SCREEN_WIDTH, 100,  // B
    };

    std::array<GLuint, 2> indices{
            0, 1,
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLuint), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(GLuint), (const void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool check_collision_x(int rectangle_front, int rectangle_back, int triangle_front, int triangle_back)
{
    if (rectangle_front >= triangle_front && rectangle_back <= triangle_back)
    {
        return true;
    }
    return false;
}

bool check_collision_y(int rectangle_bottom)
{
    if (rectangle_bottom <= 135)
    {
        return true;
    }
    return false;
}


bool is_space_key_pressed(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        return true;
    }
    return false;
}