#version 430

uniform sampler2D test_texture;
uniform int state; // -1: ground

out vec4 final_color;

in vec2 tex_coords;

void main() {
	final_color = texture( test_texture, tex_coords );

	if ( state == -1 ) {
		final_color = vec4( 0.941, 0.918, 0.839, 1.0 );
	}
}