cmake_minimum_required(VERSION 3.25)
project(gl_jump)

set(CMAKE_CXX_STANDARD 23)

include_directories(vendor)
include_directories(include)
include_directories(vendor/freetype-2.12.0/include)

add_executable(${PROJECT_NAME}
        src/main.cpp
        include/gl_gridlines/gl_gridlines.cpp
        include/gl_textrenderer/gl_textrenderer.cpp
        include/Shader/Shader.cpp include/Shader/Shader.h include/Rectangle/Rectangle.cpp include/Rectangle/Rectangle.h)

find_package ( glfw3 REQUIRED )
target_link_libraries(${PROJECT_NAME} PUBLIC glfw )

find_package ( glbinding REQUIRED )
target_link_libraries(${PROJECT_NAME} PUBLIC glbinding::glbinding )

find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC freetype)

# make glfw work with glbinding
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
