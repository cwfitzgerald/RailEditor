#pragma once

#include <SDL2/SDL.h>

#include "graphics.hpp"
#include "shaders.hpp"

namespace graphics {
	struct SDL_Context_t {
		SDL_Window* window;
		SDL_GLContext gl_context;
	};

	extern SDL_Context_t sdl_context;

	void initialize_sdl(std::size_t width, std::size_t height);
	void initialize_shaders();
}
