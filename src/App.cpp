#include "App.h"

#include <imgui.h>

App::App() {

}

App::~App() {

}

void App::init_debug_callback() {
	GLint context_flags;
	glGetIntegerv( GL_CONTEXT_FLAGS, &context_flags );
	if ( context_flags & SDL_GL_CONTEXT_DEBUG_FLAG ) {
		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE );
		glDebugMessageControl( GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE );
		glDebugMessageCallback( SDL_GLDebugMessageCallback, nullptr );
	}
}

void App::init_shaders() {

}

bool App::init() {
	init_debug_callback();

	camera.set_view( glm::vec3( 0.f, 2.f, 2.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec3( 0.f, 0.f, 0.f ) );

	glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );

	init_shaders();

	return true;
}

void App::clean_shaders() {

}

void App::clean() {
	clean_shaders();
}

void App::tick() {
	tick_time += tick_dt;
	std::cout << "[TICK]: " << tick_time << std::endl;
}

void App::update( const float dt ) {
	tick_accum += dt * tick_rate;
	while ( tick_accum >= tick_dt ) {
		tick();
		tick_accum -= tick_dt;
	}

	camera.update( dt );
}

void App::render() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void App::render_gui() {
	//ImGui::ShowStyleEditor();

	if ( ImGui::Begin("settings") ) {
		ImGui::SliderFloat("tick rate", &tick_rate, 0.f, 10.f, "%.3f");
	}

	ImGui::End();
}

#pragma region event handling
void App::keyboard_down( const SDL_KeyboardEvent& event ) {
	camera.keyboard_down( event );
}

void App::keyboard_up( const SDL_KeyboardEvent& event ) {
	camera.keyboard_up( event );
}

void App::mouse_down( const SDL_MouseButtonEvent& event ) {
	
}

void App::mouse_up( const SDL_MouseButtonEvent& event ) {

}

void App::mouse_scroll( const SDL_MouseWheelEvent& event ) {
	camera.mouse_scroll( event );
}

void App::mouse_move( const SDL_MouseMotionEvent& event ) {
	camera.mouse_move( event );
}

void App::resize( int width, int height ) {
	glViewport( 0, 0, width, height );
}

void App::other_event( const SDL_Event& event ) {
	
}
#pragma endregion