#pragma once

#include <SDL2/SDL.h>
#include <cstdlib>

namespace resystem {
	struct SDL_Context_t {
		SDL_Window* window;
		SDL_GLContext gl_context;
		std::size_t width, height;
	};

	extern SDL_Context_t sdl_context; // defined in window_management.cpp

	
	void initialize_sdl();
	void destroy_sdl();
}
