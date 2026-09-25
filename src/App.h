#pragma once

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
};