#pragma once

#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <map>

using namespace gl;

class gl_textrenderer
{
public:
    gl_textrenderer(unsigned int screen_width, unsigned int screen_height, std::string font_path, int pixel_height,
                    std::array<float, 4> colors);

    ~gl_textrenderer();

    void render_text(std::string text, float x, float y);

    std::pair<int, int> get_text_size(std::string text);

private:
    struct m_vertex
    {
        glm::vec2 position;
        glm::vec2 texture_coordinates;
    };
    struct m_character
    {
        unsigned int TextureID;  // opengl texture ID of the glyph
        glm::ivec2 Size;       // Size of glyph (width and height of bitmap)
        // bearing.x horizontal position relative to the origin
        // bearing.y vertical position relative to the baseline
        glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
        // horizontal distance in 1/64th pixels from the origin to the next origin
        unsigned int Advance;    // Offset to advance to next glyph
    };

    void load_ascii_characters(int pixel_height);

    unsigned int create_shader_program(std::string& vertex_src, std::string& fragment_src);

    std::string m_font_path;
    glm::mat4 m_projection;
    std::map<char, m_character> m_characters;
    std::array<float, 4> m_colors;

    unsigned int m_shader_program;
};
