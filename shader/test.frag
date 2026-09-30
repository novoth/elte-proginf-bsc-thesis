#version 430

out vec4 final_color;

in vec2 uv;

void main() {
	final_color = vec4( uv.x - int( uv.x ), uv.y - int( uv.y ), 0.f, 1.f );
}