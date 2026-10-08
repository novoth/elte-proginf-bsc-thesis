#version 430 core

uniform sampler2D used_texture;

in vec2 tex_coords;

out vec4 final_color;

void main() {
	final_color = texture( used_texture, vec2( ( tex_coords.x - 0.5 ) * 0.85 + 0.5, ( tex_coords.y - 0.5 ) * 0.85 + 0.5 ) );

	if ( final_color.a < 0.5 ) {
		discard;
	}

	final_color += vec4( 0.2, 0.2, 0.2, 1.0 );
	final_color.r = min( 1.0, final_color.r );
	final_color.g = min( 1.0, final_color.g );
	final_color.b = min( 1.0, final_color.b );
}