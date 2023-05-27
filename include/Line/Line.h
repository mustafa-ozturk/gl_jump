#pragma once

#include <iostream>
#include <glbinding/gl/gl.h>

using namespace gl;

class Line
{
public:
    Line() = default;
    ~Line() = default;
    void draw(unsigned int shader_program, float r, float g, float b,
              GLuint start_x, GLuint start_y, GLuint end_x, GLuint end_y);
};
