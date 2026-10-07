#version 430 core

uniform sampler2D used_texture;

in vec2 tex_coords;

out vec4 final_color;

void main() {
//	final_color = vec4( 0.0, 0.0, 1.0, 1.0 );
	final_color = texture( used_texture, tex_coords );
	if ( final_color.a < 0.5 ) {
		discard;
	}
}