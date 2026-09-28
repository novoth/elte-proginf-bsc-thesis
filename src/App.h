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

	bool init();
	void clean();

	void update( const float dt );
	void render();
	void render_gui();

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
	void init_debug_callback();

	void init_shaders();
	void clean_shaders();

	Camera camera;

	#pragma region tick
	void tick();
	float tick_rate = 1.f;
	float tick_dt = .1f;
	float tick_accum = 0.f;
	float tick_time = 0.f;
	#pragma endregion
};