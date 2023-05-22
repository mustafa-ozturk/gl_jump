#pragma once

#include <iostream>
#include <glbinding/gl/gl.h>
using namespace gl;

class Triangle
{
public:
    Triangle(int triangle_width, int triangle_height, int triangle_pos_x, int triangle_pos_y);
    ~Triangle();
    void draw();
    int triangle_width = 0;
    int triangle_height = 0;
    int triangle_pos_x = 0;
    int triangle_pos_y = 0;
};
