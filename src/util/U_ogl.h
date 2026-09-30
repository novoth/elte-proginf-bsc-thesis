#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include <filesystem>

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <SDL3_image/SDL_image.h>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

struct OGL_obj {
	GLuint vbo_id = 0;
	GLuint ibo_id = 0;
	GLuint vao_id = 0;
	GLsizei count = 0;
};

GLuint attach_shader( const GLuint program_id, GLenum shader_type, const std::filesystem::path& file_name );
void link_program( const GLuint program_id );
OGL_obj create_obj_from_mesh( const Mesh& mesh );
void clean_ogl_obj( OGL_obj& ogl_obj );
GLint uniform_location( const GLchar* uniform_name );