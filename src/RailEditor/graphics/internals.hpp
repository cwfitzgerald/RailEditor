#pragma once

#include <SDL2/SDL.h>

#include "graphics.hpp"
#include "shaders.hpp"

namespace graphics {
	struct SDL_Context_t {
		SDL_Window* window;
		SDL_GLContext gl_context;
		std::size_t width, height;
	};

	struct OpenGL_Extentions_t {
		bool tesselation = false;
	};
	extern OpenGL_Extentions_t opengl_extentions; // defined in window_management.cpp

	extern SDL_Context_t sdl_context; // defined in window_management.cpp

	void initialize_sdl();
	void initialize_ogl();
}
