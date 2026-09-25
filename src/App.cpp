#include "App.h"

#include <imgui.h>

App::App() {

}

App::~App() {

}

bool App::init() {
	glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );

	return true;
}

void App::clean() {
	
}

void App::update( const float dt ) {
	
}

void App::render() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void App::render_gui() {
	ImGui::ShowStyleEditor();
}

#pragma region event handling
void App::keyboard_down( const SDL_KeyboardEvent& event ) {
	float r = ((float)rand() / RAND_MAX);
	float g = ((float)rand() / RAND_MAX);
	float b = ((float)rand() / RAND_MAX);

	float l = glm::sqrt(r * r + g * g + b * b);
	if (l < 0.1f) {
		return;
	}

	glClearColor(r / l, g / l, b / l, 1.0f);
}

void App::keyboard_up( const SDL_KeyboardEvent& event ) {

}

void App::mouse_down( const SDL_MouseButtonEvent& event ) {

}

void App::mouse_up( const SDL_MouseButtonEvent& event ) {

}

void App::mouse_scroll( const SDL_MouseWheelEvent& event ) {

}

void App::mouse_move( const SDL_MouseMotionEvent& event ) {

}

void App::resize( int width, int height ) {
	glViewport( 0, 0, width, height );
}

void App::other_event( const SDL_Event& event ) {
	
}
#pragma endregion