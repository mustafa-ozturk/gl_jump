#include "Triangle.h"

Triangle::Triangle(int triangle_width, int triangle_height, int triangle_pos_x,
                   int triangle_pos_y)
        : triangle_width(triangle_width),
          triangle_height(triangle_height),
          triangle_pos_x(triangle_pos_x),
          triangle_pos_y(triangle_pos_y)
{

}

Triangle::~Triangle()
{

}

void Triangle::draw(unsigned int shader_program, float r, float g, float b)
{
    glUseProgram(shader_program);
    glUniform3f(glGetUniformLocation(shader_program, "color"), r, g, b);
    /*
    *     B
    *    / \
    *   A - C
    */
    std::array<int, 8> vertices{
            triangle_pos_x, triangle_pos_y,                     // A
            triangle_pos_x + triangle_width / 2,
            triangle_pos_y + triangle_height,   // B
            triangle_pos_x + triangle_width,
            triangle_pos_y,                     // C
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLuint),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(int),
                          (const void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int),
                 indices.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Triangle::update_position(int score, double delta_time,
                               unsigned int screen_width, int reset_pos)
{
    triangle_pos_x -= (300 + score) * delta_time;
    if (triangle_pos_x < reset_pos)
    {
        triangle_pos_x = screen_width;
    }
}
