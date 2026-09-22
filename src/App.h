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

	void update( float dt );
	void render();
	void render_gui();
};