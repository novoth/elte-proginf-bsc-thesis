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
	test_shader = glCreateProgram();
	attach_shader(test_shader, GL_VERTEX_SHADER, "shader/test.vert");
	attach_shader(test_shader, GL_FRAGMENT_SHADER, "shader/test.frag");
	link_program(test_shader);

}

bool App::init() {
	init_debug_callback();

	camera.set_view( glm::vec3( 0.f, 5.f, 2.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec3( 0.f, 0.f, 0.f ) );

	glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );

	init_shaders();

	Mesh test_gpu = {
		std::vector<Vertex>{
			{ glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
			{ glm::vec3( 0.f, 0.f, 10.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 10.f, 0.f ) },
			{ glm::vec3( 10.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 10.f ) },
			{ glm::vec3( 10.f, 0.f, 10.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 10.f, 10.f ) },
		},
		std::vector<GLuint>{
			0, 1, 2,
			2, 1, 3
		}
	};
	test = create_obj_from_mesh( test_gpu );

	return true;
}

void App::clean_shaders() {

}

void App::clean() {
	clean_shaders();
	clean_ogl_obj( test );
}

void App::tick() {
	tick_time += tick_dt;
}

void App::update( const float dt ) {
	tick_accum += dt * tick_rate;
	while ( tick_accum >= tick_dt ) {
		tick();
		tick_accum -= tick_dt;
	}

	camera.update( dt );
}

void App::set_common_uniforms() {
	glm::mat4 view_proj_mtx = camera.get_view_proj_mtx();
	glUniformMatrix4fv( uniform_location( "view_proj" ), 1, GL_FALSE, glm::value_ptr( view_proj_mtx ) );
}

void App::draw_ogl_obj(OGL_obj ogl_obj, const glm::mat4& world_mtx ) {
	glUniformMatrix4fv( uniform_location( "world" ), 1, GL_FALSE, glm::value_ptr( world_mtx ) );
	glUniformMatrix4fv( uniform_location( "world_i_t" ), 1, GL_FALSE, glm::value_ptr( glm::transpose( glm::inverse( world_mtx ) ) ) );
	glBindVertexArray( ogl_obj.vao_id );
	glDrawElements( GL_TRIANGLES, ogl_obj.count, GL_UNSIGNED_INT, nullptr );
}

void App::render() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUseProgram( test_shader );
	set_common_uniforms();

	draw_ogl_obj( test, glm::translate( glm::mat4( 1.f ), glm::vec3( -5.f, 0.f, -5.f ) ) );
}

void App::render_gui() {
	//ImGui::ShowStyleEditor();

	if ( ImGui::Begin( "settings" ) ) {
		ImGui::SliderFloat( "tick rate", &tick_rate, 0.f, 10.f, "%.1f" );
		ImGui::SliderFloat( "sensitivity", &( camera.sens ), 10.f, 200.f, "%.0f" );
		ImGui::SliderFloat( "camera speed", &( camera.speed ), 1.f, 20.f, "%.1f" );
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