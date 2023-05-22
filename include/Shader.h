#pragma once

#include <iostream>
#include <glbinding/gl/gl.h>
using namespace gl;

class Shader
{
public:
    Shader();
    ~Shader();
    unsigned int get_shader_program();
private:
    unsigned int create_shader_program(const std::string& vertex_source, const std::string& fragment_source);
    unsigned int m_shader_program;
    std::string m_vertex_shader_source;
    std::string m_fragment_shader_source;
};
