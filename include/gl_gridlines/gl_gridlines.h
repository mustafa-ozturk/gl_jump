#pragma once

#include <iostream>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace gl;

class gl_gridlines
{
public:
    gl_gridlines(unsigned int screen_width, unsigned int screen_height, unsigned int grid_size,
                 std::array<float, 3> line_colors);

    ~gl_gridlines();

    void draw();

private:
    struct m_vertex {
        glm::ivec3 position;
        float color_alpha;
    };
    unsigned int m_screen_width;
    unsigned int m_screen_height;
    unsigned int m_shader_program;

    unsigned int m_vbo, m_vao, m_ebo;
    std::vector<m_vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    unsigned int m_grid_size;
    std::array<float, 3> m_line_colors;
    unsigned int m_lines = 0;

    unsigned int create_shader_program(const std::string& vertex_source, const std::string& fragment_source);

    void create_gridline_data();

    void setup_gl_objects();

    void set_projection_view();
};
