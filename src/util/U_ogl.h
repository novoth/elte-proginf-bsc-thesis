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
#include <functional>
#include <cstdint>

#include <chrono>

typedef std::uint8_t marker_id;

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

struct Marker_moved_arg {
	marker_id id;
	glm::vec2 delta;
};

struct Marker {
	explicit Marker( glm::vec2* to_update = nullptr, std::function< void( Marker_moved_arg ) > notify_function = nullptr ) : update_pos( to_update ), notify_on_update( std::move( notify_function ) ) {
		notify_id = 0;

		if ( to_update != nullptr ) {
			set_base_pos( *to_update );
		}
	}

	void set_base_pos( glm::vec2 new_value, bool call_notify_function = true ) {
		if ( base_pos == new_value ) {
			return;
		}

		glm::vec2 delta = new_value - base_pos;
		base_pos = new_value;

		if ( update_pos != nullptr ) {
			*update_pos = new_value;
		}
		if ( call_notify_function && notify_on_update != nullptr ) {
			notify_on_update( Marker_moved_arg { notify_id, delta } );
		}
	}

	void set_offset( glm::vec2 new_offset ) {
		offset = new_offset;
	}

	glm::vec2 get_offset() {
		return offset;
	}

	glm::vec2 get_ground_pos() {
		return base_pos + offset;
	}

	glm::vec2 get_base_pos() { return base_pos; }

	void set_notify_function( std::function< void( Marker_moved_arg ) > new_notify_function ) {
		notify_on_update = new_notify_function;
	}

	void set_id( marker_id new_notify_id ) {
		notify_id = new_notify_id;
	}

	marker_id get_id() {
		return notify_id;
	}
		
private:
	marker_id notify_id;

	glm::vec2 offset = glm::vec2( 0.f, 0.f );
	glm::vec2 base_pos = glm::vec2( 0.f, 0.f );
	glm::vec2 ground_pos = glm::vec2( 0.f, 0.f );
	glm::vec2 *update_pos = nullptr;

	std::function< void( Marker_moved_arg ) > notify_on_update;
};

struct Int_vec_pair {
	int idx;
	glm::vec2 pos;
};

#pragma region key modifiers
enum class Modifier : unsigned char {
	none = 0,
	alt =	1 << 0,
	ctrl =	1 << 1,
	shift =	1 << 2,

	ctrl_alt = ctrl | alt,
	ctrl_shift = ctrl | shift,
	alt_shift = alt | shift,
	ctrl_alt_shift = ctrl | alt | shift,
};

constexpr Modifier operator&( Modifier a, Modifier b );
constexpr Modifier operator&( Modifier a, Modifier b );
constexpr Modifier operator^( Modifier a, Modifier b );
constexpr Modifier operator~( Modifier a );
bool key_mod_used( Modifier current_mod, Modifier check );
void key_mod_set( Modifier& current_mod, Modifier to_set );
void key_mod_remove( Modifier& current_mod, Modifier to_remove );
void key_mod_toggle( Modifier& current_mod, Modifier to_remove );
#pragma endregion

GLuint attach_shader( const GLuint program_id, GLenum shader_type, const std::filesystem::path& file_name );
void link_program( const GLuint program_id );
OGL_obj create_obj_from_mesh( const Mesh& mesh );
void clean_ogl_obj( OGL_obj& ogl_obj );
GLint uniform_location( const GLchar* uniform_name );
GLsizei mip_level_count( const Img& img );
Img load_img_from_file( const std::filesystem::path& file_name );
char orientation_char_from_camera_u( const float camera_u );
bool ray_hit_plane( const Ray ray, const glm::vec3 plane_point, const glm::vec3 plane_u, const glm::vec3 plane_v, Ray_intersection& intersection );
bool ray_hit_ground_plane( const Ray ray, glm::vec2& position );
glm::vec2 bezier( const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const float t );
glm::vec2 bezier_1der( const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const float t );
glm::vec2 bezier_2der( const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const float t );
glm::vec2 rot_right( const glm::vec2& v );
glm::vec2 lerp_vec2( const glm::vec2 point_a, const glm::vec2 point_b, const float t );
glm::vec2 s_mult( const float& s, const glm::vec2& v );
marker_id fill_marker_id( const unsigned int& segment_id, const unsigned int& point_id );
void read_marker_id( const marker_id& id, unsigned int& segment_id, unsigned int& point_id );
std::vector< GLuint > triangulate_counterclockwise( const std::vector< glm::vec2 >& points );
bool is_convex_triangle( const glm::vec2& prev, const glm::vec2& current, const glm::vec2& next );
template <typename T>
int safe_index_vector( const std::vector< T >& vector, int idx );
bool triangle_contains_point( const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& point );