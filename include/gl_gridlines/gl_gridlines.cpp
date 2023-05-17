#include "gl_gridlines.h"

gl_gridlines::gl_gridlines(unsigned int screen_width, unsigned int screen_height, unsigned int grid_size,
                           std::array<float, 3> line_colors)
        : m_screen_width(screen_width), m_screen_height(screen_height),
          m_grid_size(grid_size), m_line_colors(line_colors)
{
    const std::string vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec3 pos;
        layout (location = 1) in float color_alpha;

        out float v_alpha;

        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * vec4(pos.xyz, 1.0);
            v_alpha = color_alpha;
        }
    )";
    const std::string fragment_shader_source = R"(
        #version 330 core

        out vec4 FragColor;

        in float v_alpha;

        uniform vec3 color;

        void main()
        {
            FragColor = vec4(color, v_alpha);
        }
    )";
    m_shader_program = create_shader_program(vertex_shader_source, fragment_shader_source);

    create_gridline_data();
    setup_gl_objects();
    set_projection_view();
}

gl_gridlines::~gl_gridlines()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteProgram(m_shader_program);
}

void gl_gridlines::draw()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(m_shader_program);
    glBindVertexArray(m_vao);
    glDrawElements(GL_LINES, m_lines * 2, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

unsigned int gl_gridlines::create_shader_program(const std::string& vertex_source, const std::string& fragment_source)
{
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* c_str_vertex = vertex_source.c_str();
    glShaderSource(vertexShader, 1, &c_str_vertex, nullptr);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* c_str_fragment = fragment_source.c_str();
    glShaderSource(fragmentShader, 1, &c_str_fragment, nullptr);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void gl_gridlines::create_gridline_data()
{
    // vertical lines
    for (int i = m_grid_size; i < m_screen_height; i += m_grid_size)
    {
        m_vertex first_line_point = {};
        first_line_point.position = {0, i, 1 };
        first_line_point.color_alpha = 0.1f;
        m_verticies.push_back(first_line_point);

        m_vertex second_line_point = {};
        second_line_point.position = {m_screen_width, i, 1 };
        second_line_point.color_alpha = 0.1f;
        m_verticies.push_back(second_line_point);

        m_lines++;
    }

    // horizontal lines
    for (int i = m_grid_size; i < m_screen_width; i += m_grid_size)
    {
        m_vertex first_line_point = {};
        first_line_point.position = {i, 0, 1 };
        first_line_point.color_alpha = 0.1f;
        m_verticies.push_back(first_line_point);

        m_vertex second_line_point = {};
        second_line_point.position = {i, m_screen_height, 1 };
        second_line_point.color_alpha = 0.1f;
        m_verticies.push_back(second_line_point);

        m_lines++;
    }

    // horizontal center line
    m_vertex hz_first_line_point = {};
    hz_first_line_point.position = {m_screen_width / 2, 0, 1 };
    hz_first_line_point.color_alpha = 1.0f;
    m_verticies.push_back(hz_first_line_point);

    m_vertex hz_second_line_point = {};
    hz_second_line_point.position = {m_screen_width / 2, m_screen_height, 1};
    hz_second_line_point.color_alpha = 1.0f;
    m_verticies.push_back(hz_second_line_point);
    m_lines++;

    // vertical center line
    m_vertex vrt_first_line_point = {};
    vrt_first_line_point.position = {0, m_screen_height / 2, 1};
    vrt_first_line_point.color_alpha = 1.0f;
    m_verticies.push_back(vrt_first_line_point);

    m_vertex vrt_second_line_point = {};
    vrt_second_line_point.position = {m_screen_width, m_screen_height / 2, 1};
    vrt_second_line_point.color_alpha = 1.0f;
    m_verticies.push_back(vrt_second_line_point);
    m_lines++;

    for (int i = 0; i < m_lines * 2; i++)
    {
        m_indices.push_back(i);
    }
}

void gl_gridlines::setup_gl_objects()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_verticies.size() * sizeof(m_vertex), m_verticies.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, sizeof(m_vertex), (const void*)offsetof(m_vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(m_vertex), (const void*)offsetof(m_vertex, color_alpha));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void gl_gridlines::set_projection_view()
{
    glm::mat4 projection = glm::ortho(0.0f, (float) m_screen_width, 0.0f, (float) m_screen_height);
    glUseProgram(m_shader_program);
    glUniformMatrix4fv(glGetUniformLocation(m_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(m_shader_program, "color"), m_line_colors[0], m_line_colors[1], m_line_colors[2]);
}


