#pragma once

#include <iostream>
#include <glbinding/gl/gl.h>
using namespace gl;

class Rectangle
{
public:
    Rectangle(int rectangle_width, int rectangle_height, int rectangle_pos_x, int rectangle_pos_y);
    ~Rectangle();
    void draw();
    int rectangle_width = 0;
    int rectangle_height = 0;
    int rectangle_pos_x = 0;
    int rectangle_pos_y = 0;
    bool jump = false;
    int jump_amount = 10;
};
