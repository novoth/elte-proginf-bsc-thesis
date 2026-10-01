#version 430 core

layout( location = 0 ) in vec3 vbo_obj_space_pos;
layout( location = 1 ) in vec3 vbo_obj_space_normal;
layout( location = 2 ) in vec2 vbo_tex_coords;

out vec2 tex_coords;

uniform mat4 view_proj;
uniform mat4 world;
uniform mat4 world_i_t;

void main() {
	vec4 world_pos = world * vec4( vbo_obj_space_pos, 1 );
	gl_Position = view_proj * world_pos;
	tex_coords = world_pos.xz;
}