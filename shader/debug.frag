#version 430 core

in vec2 tex_coords;

out vec4 final_color;

void main() {
    vec4 r = vec4( 0.0, 0.0, 0.0, 1.0 );
	if ( tex_coords.x < 0.5 ) {
		r = vec4( 1.0, 1.0, 0.0, 1.0 );
	}
	final_color = r;
}