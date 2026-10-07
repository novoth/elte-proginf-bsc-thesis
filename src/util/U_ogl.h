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

struct Img {
	std::vector<glm::u8vec4> pixel_data;
	unsigned int width = 0;
	unsigned int height = 0;

	void assign( const glm::uint32* pixels, unsigned int width, unsigned int height ) {
		this->width = width;
		this->height = height;

		const glm::u8vec4* _pixel_data = reinterpret_cast<const glm::u8vec4*>( pixels );
		pixel_data.assign( _pixel_data, _pixel_data + width * height );
	}

	const glm::u8vec4* data() const {
		return pixel_data.data();
	}
};

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

struct Ray_intersection {
	glm::vec2 uv;
	float t;
};

GLuint attach_shader( const GLuint program_id, GLenum shader_type, const std::filesystem::path& file_name );
void link_program( const GLuint program_id );
OGL_obj create_obj_from_mesh( const Mesh& mesh );
void clean_ogl_obj( OGL_obj& ogl_obj );
GLint uniform_location( const GLchar* uniform_name );
GLsizei mip_level_count( const Img& img );
Img load_img_from_file( const std::filesystem::path& file_name );
char orientation_char_from_camera_u( const float camera_u );
bool ray_hit_plane( const Ray ray, const glm::vec3 plane_point, const glm::vec3 plane_u, const glm::vec3 plane_v, Ray_intersection& intersection );
bool ray_hit_ground_plane( const Ray ray, glm::vec3& position );