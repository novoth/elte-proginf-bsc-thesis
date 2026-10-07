#include "App.h"

#include <imgui.h>

App::App() {
	camera = new Camera( this );
}

App::~App() {
	
}

#pragma region initialization
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

	non_lit_ground_sdr_with_local_texturing = glCreateProgram();
	attach_shader(non_lit_ground_sdr_with_local_texturing, GL_VERTEX_SHADER, "shader/non_lit_ground_sdr_with_local_textures.vert");
	attach_shader(non_lit_ground_sdr_with_local_texturing, GL_FRAGMENT_SHADER, "shader/non_lit_ground_sdr_with_local_textures.frag");
	link_program(non_lit_ground_sdr_with_local_texturing);
}

void App::init_textures() {
	glCreateSamplers( 1, &pixel_2d_sampler_id );
	glSamplerParameteri( pixel_2d_sampler_id, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glSamplerParameteri( pixel_2d_sampler_id, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glSamplerParameteri( pixel_2d_sampler_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( pixel_2d_sampler_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	Img test_img = load_img_from_file( "asset/test.png" );
	glCreateTextures( GL_TEXTURE_2D, 1, &test_texture_id );
	glTextureStorage2D( test_texture_id, mip_level_count( test_img ), GL_RGBA8, test_img.width, test_img.height );
	glTextureSubImage2D( test_texture_id, 0, 0, 0, test_img.width, test_img.height, GL_RGBA, GL_UNSIGNED_BYTE, test_img.data() );
	glGenerateTextureMipmap( test_texture_id );

	Img marker_img = load_img_from_file( "asset/marker.png" );
	glCreateTextures( GL_TEXTURE_2D, 1, &marker_texture_id );
	glTextureStorage2D( marker_texture_id, mip_level_count( marker_img ), GL_RGBA8, marker_img.width, marker_img.height );
	glTextureSubImage2D( marker_texture_id, 0, 0, 0, marker_img.width, marker_img.height, GL_RGBA, GL_UNSIGNED_BYTE, marker_img.data() );
	glGenerateTextureMipmap( marker_texture_id );
}

void App::init_geometry() {
	Mesh ground_gpu = {
		std::vector<Vertex>{
			{ glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
			{ glm::vec3( 0.f, 0.f, 1.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
			{ glm::vec3( 1.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
			{ glm::vec3( 1.f, 0.f, 1.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
		},
		std::vector<GLuint>{
			0, 1, 2,
			2, 1, 3,
		}
	};
	ground = create_obj_from_mesh( ground_gpu );

	Mesh test_gpu = {
		std::vector<Vertex>{
			{ glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
			{ glm::vec3( 0.f, 0.f, 1.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
			{ glm::vec3( 10.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
			{ glm::vec3( 1.f, 0.f, 2.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
		},
		std::vector<GLuint>{
			0, 1, 2,
			2, 1, 3,
		}
	};
	test = create_obj_from_mesh( test_gpu );

	Mesh marker_gpu = {
		std::vector<Vertex>{
			{ glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 0.f ) },
			{ glm::vec3( 0.f, 0.f, 1.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 0.f, 1.f ) },
			{ glm::vec3( 1.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 1.f, 0.f ) },
			{ glm::vec3( 1.f, 0.f, 1.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec2( 1.f, 1.f ) },
	},
		std::vector<GLuint>{
			0, 1, 2,
			2, 1, 3,
	}
	};
	marker = create_obj_from_mesh( marker_gpu );
}

bool App::init() {
	init_debug_callback();

	camera->set_view( glm::vec3( 0.f, 5.f, 2.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec3( 0.f, 0.f, 0.f ) );

	glClearColor( 0.627f, 0.835f, 0.922f, 1.0f );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );

	init_shaders();
	init_textures();
	init_geometry();

	return true;
}
#pragma endregion

#pragma region cleanup
void App::clean_shaders() {

}

void App::clean_textures() {
	glDeleteSamplers( 1, &pixel_2d_sampler_id );

	glDeleteTextures( 1, &test_texture_id );
}

void App::clean_geometry() {
	clean_ogl_obj( test );
	clean_ogl_obj( ground );
}

void App::clean() {
	clean_shaders();
	clean_textures();
	clean_geometry();
}
#pragma endregion

void App::tick() {
	tick_time += tick_dt;
}

void App::update( const float dt ) {
	tick_accum += dt * tick_rate;
	app_time += dt;
	while ( tick_accum >= tick_dt ) {
		tick();
		tick_accum -= tick_dt;
	}

	camera->update( dt );
}

#pragma region rendering
void App::set_common_uniforms() {
	glm::mat4 view_proj_mtx = camera->get_view_proj_mtx();
	glUniformMatrix4fv( uniform_location( "view_proj" ), 1, GL_FALSE, glm::value_ptr( view_proj_mtx ) );
}

void App::draw_ogl_obj(OGL_obj ogl_obj, const glm::mat4& world_mtx ) {
	glUniformMatrix4fv( uniform_location( "world" ), 1, GL_FALSE, glm::value_ptr( world_mtx ) );
	glUniformMatrix4fv( uniform_location( "world_i_t" ), 1, GL_FALSE, glm::value_ptr( glm::transpose( glm::inverse( world_mtx ) ) ) );
	glBindVertexArray( ogl_obj.vao_id );
	glDrawElements( GL_TRIANGLES, ogl_obj.count, GL_UNSIGNED_INT, nullptr );
}

void App::render_latest_marker() {
	//	if (!camera->topdown) { return; }

	glUseProgram( non_lit_ground_sdr_with_local_texturing );
	glBindSampler( 0, pixel_2d_sampler_id );

	set_common_uniforms();

	glBindTextureUnit( 0, marker_texture_id );
	glUniform1i( uniform_location( "used_texture" ), 0 );

	draw_ogl_obj( marker, glm::translate( glm::mat4( 1.f ), last_ground_intersection + glm::vec3( -.5f, 0.f, -.5f ) ) );
}

void App::render_ground() {
	glUseProgram( test_shader );

	set_common_uniforms();

	glUniform1i( uniform_location( "state" ), -1 );

	glm::vec3 ground_quad_transform = camera->get_look_at();
	ground_quad_transform.y = 0.f;
	ground_quad_transform += glm::vec3( -.5f, 0, -.5f ) * ground_size;
	draw_ogl_obj( ground, glm::scale( glm::translate( glm::mat4( 1.f ), ground_quad_transform ), glm::vec3( 1.f, 1.f, 1.f) * ground_size ) );
}

void App::render_test() {
	glUseProgram( test_shader );
	glBindSampler( 0, pixel_2d_sampler_id );

	set_common_uniforms();

	glBindTextureUnit( 0, test_texture_id );
	glUniform1i( uniform_location( "test_texture" ), 0 );
	glUniform1i( uniform_location( "state" ), 0 );

	draw_ogl_obj( test, glm::translate( glm::mat4( 1.f ), glm::vec3( 0.f, 0.f, 0.f) ) );
}

void App::render() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	#pragma region render 2d ground
	glDisable( GL_DEPTH_TEST );

	render_ground();
	render_test();
	render_latest_marker();
	
	glEnable( GL_DEPTH_TEST );
	#pragma endregion

	glBindTextureUnit( 0, 0 );
	glBindSampler( 0, 0 );
	glBindVertexArray( 0 );
	glUseProgram( 0 );
}

void App::render_gui() {
	//ImGui::ShowStyleEditor();

	if ( ImGui::Begin( "Info" ) ) {
		ImGui::Text( "FPS: %.1f", ImGui::GetIO().Framerate );
		ImGui::Text( "Facing: %c", orientation_char_from_camera_u( camera->u ) );
		ImGui::Text( "Mouse pos: %.0f , %.0f", mouse_pos.x, mouse_pos.y );

		ImGui::Separator();

		float camera_fov_deg = glm::degrees( camera->get_fov_y() );
		if ( ImGui::SliderFloat( "FOV", &camera_fov_deg, 10.f, 150.f, "%.0f" ) ) {
			camera->set_fov_y( glm::radians( camera_fov_deg ) );
		}
	}

	ImGui::End();
}
#pragma endregion

#pragma region event handling
void App::keyboard_down( const SDL_KeyboardEvent& event ) {
	camera->keyboard_down( event );
}

void App::keyboard_up( const SDL_KeyboardEvent& event ) {
	camera->keyboard_up( event );
}

void App::mouse_down( const SDL_MouseButtonEvent& event ) {
	if ( event.button == SDL_BUTTON_LMASK ) {
		if ( ray_hit_ground_plane( camera->get_ray_through_pixel( mouse_pos, window_size ), mouse_ground_intersection ) ) {
			last_ground_intersection = mouse_ground_intersection;
		}
	}
}

void App::mouse_up( const SDL_MouseButtonEvent& event ) {

}

void App::mouse_scroll( const SDL_MouseWheelEvent& event ) {
	camera->mouse_scroll( event );
}

void App::mouse_move( const SDL_MouseMotionEvent& event ) {
	mouse_pos.x = event.x;
	mouse_pos.y = event.y;

	camera->mouse_move( event );
}

void App::resize( int width, int height ) {
	window_size = glm::vec2( width, height );

	glViewport( 0, 0, width, height );
	camera->set_aspect( (float)width / height );
}

void App::other_event( const SDL_Event& event ) {
	
}
#pragma endregion