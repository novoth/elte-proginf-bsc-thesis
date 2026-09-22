#include <GL/glew.h>

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include "App.h"

int main( int argc, char* args[] ) {
	
	// SDL init
	SDL_SetLogPriority( SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR );
	if ( !SDL_Init( SDL_INIT_VIDEO ) ) {
		SDL_LogError( SDL_LOG_CATEGORY_ERROR, "Error occured during SDL initialization: %s", SDL_GetError() );
		return 1;
	}
	std::atexit( SDL_Quit );

	// SDL OpenGL
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG ); //DEBUG mode only?

	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE,    8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,  8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,   8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE,  8 );

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	
	SDL_Window* window = nullptr;
	window = SDL_CreateWindow( "Traffic Simulation - Novoth Botond", 640, 360, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	if ( window == nullptr ) {
		SDL_LogError( SDL_LOG_CATEGORY_ERROR, "Error occured during SDL initialization: %s", SDL_GetError() );
		return 1;
	}

	// OpenGL context
	SDL_GLContext ogl_context = SDL_GL_CreateContext( window );
	if ( ogl_context == nullptr ) {
		SDL_LogError( SDL_LOG_CATEGORY_ERROR, "Error occured during openGL context creation: %s", SDL_GetError() );
		return 2;
	}

	SDL_GL_SetSwapInterval( 1 );

	GLenum error = glewInit();
	if ( error != GLEW_OK ) {
		SDL_LogError( SDL_LOG_CATEGORY_ERROR, "Error occured during glew initiation." );
		return 3;
	}

	int gl_version[2] = { -1, -1 };
	glGetIntegerv( GL_MAJOR_VERSION, &gl_version[0] );
	glGetIntegerv( GL_MINOR_VERSION, &gl_version[1] );

	SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "Running application with openGL version %d.%d", gl_version[0], gl_version[1] );

	if ( gl_version[0] == -1 || gl_version[1] == -1 ) {
		SDL_GL_DestroyContext( ogl_context );
		SDL_DestroyWindow( window );
		SDL_LogError( SDL_LOG_CATEGORY_ERROR, "Error occured during openGL context creation: could not get openGL version" );
		return 2;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForOpenGL( window, ogl_context );
	ImGui_ImplOpenGL3_Init();

	ImGui::StyleColorsLight();

	// Application loop
	{
		App app;
		if ( !app.init() ) {
			SDL_GL_DestroyContext( ogl_context );
			SDL_DestroyWindow( window );
			SDL_LogError( SDL_LOG_CATEGORY_ERROR, "Error occured during app initialization." );
			return 4;
		}

		bool should_quit = false;
		bool show_imgui = true;

		while ( !should_quit ) {
			//TODO: event polling

			static Uint64 last_tick = SDL_GetTicks();
			Uint64 current_tick = SDL_GetTicks();
			float dt = static_cast<float>(current_tick - last_tick) / 1000.0f;

			app.update( dt );
			app.render();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL3_NewFrame();

			ImGui::NewFrame();
			if ( show_imgui ) {
				app.render_gui();
			}
			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			SDL_GL_SwapWindow( window );
		}

		app.clean();
	}

	// Quit
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext( ogl_context );
	SDL_DestroyWindow( window );

	return 0;
}