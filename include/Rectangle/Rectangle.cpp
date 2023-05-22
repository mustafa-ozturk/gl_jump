#include "Rectangle.h"

Rectangle::Rectangle(int rectangle_width, int rectangle_height, int rectangle_pos_x, int rectangle_pos_y)
: rectangle_width(rectangle_width),
  rectangle_height(rectangle_height),
  rectangle_pos_x(rectangle_pos_x),
  rectangle_pos_y(rectangle_pos_y)
{

}

Rectangle::~Rectangle()
{

}

void Rectangle::draw()
{
    /*
    *   B - C
    *   | / |
    *   A - D
    */
    std::array<int, 8> vertices{
            rectangle_pos_x, rectangle_pos_y,                                         // A
            rectangle_pos_x, rectangle_pos_y + rectangle_height,                      // B
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

