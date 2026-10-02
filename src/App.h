#pragma once

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/U_ogl.h"
#include "util/SDL_GLDebugMessageCallback.h"

#include "util/Camera.h"

class App
{
public:
	App();
	~App();

	#pragma region initialization/cleanup
	bool init();
	void clean();
	#pragma endregion

	void update( const float dt );

	#pragma region rendering
	void render();
	void render_gui();
	#pragma endregion

	#pragma region event handling
	void keyboard_down( const SDL_KeyboardEvent& event );
	void keyboard_up( const SDL_KeyboardEvent& event );

	void mouse_down( const SDL_MouseButtonEvent& event );
	void mouse_up( const SDL_MouseButtonEvent& event );
	void mouse_scroll( const SDL_MouseWheelEvent& event );
	void mouse_move( const SDL_MouseMotionEvent& event );

	void resize( int width, int height );

	void other_event( const SDL_Event& event );
	#pragma endregion

protected:

	#pragma region initialization/cleanup
	void init_debug_callback();

	void init_shaders();
	void clean_shaders();

	void init_textures();
	void clean_textures();

	void init_geometry();
	void clean_geometry();
	#pragma endregion

	#pragma region rendering
	void set_common_uniforms();
	void draw_ogl_obj( OGL_obj ogl_obj, const glm::mat4& world_mtx );
	void render_ground();
	void render_test();
	#pragma endregion

	#pragma region tick
	void tick();
	float tick_rate = 1.f;
	float tick_dt = .1f;
	float tick_accum = 0.f;
	float tick_time = 0.f;
	#pragma endregion

	float app_time = 0.f;
	const float ground_size = 1000.f;

	Camera camera;

	OGL_obj test = {};
	OGL_obj ground = {};
	GLuint test_shader = 0;

	GLuint pixel_2d_sampler_id = 0;
	GLuint test_texture_id = 0;
	GLuint marker_texture_id = 0;

	int win_width = 0;
	int win_height = 0;
	glm::vec2 mouse_pos = glm::vec2( 0.f );
};