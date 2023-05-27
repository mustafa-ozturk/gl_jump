#pragma once

#include <iostream>
#include <glbinding/gl/gl.h>
using namespace gl;

class Triangle
{
public:
    Triangle(int triangle_width, int triangle_height, int triangle_pos_x, int triangle_pos_y);
    ~Triangle();
    void draw(unsigned int shader_program, float r, float g, float b);
    void update_position(int score, double delta_time, unsigned int screen_width, int reset_pos);
    int triangle_width = 0;
    int triangle_height = 0;
    int triangle_pos_x = 0;
    int triangle_pos_y = 0;
};
