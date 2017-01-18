#include "graphics/internals.hpp"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

struct SDL_Context_t {
	SDL_Window* window;
	SDL_GLContext gl_context;
} sdl_context;

void graphics::initialize_renderer(std::size_t width, std::size_t height) {
	graphics::initialize_sdl(width, height);
	graphics::Shader<graphics::ShaderType::vertex> vert;
}

void graphics::initialize_sdl(std::size_t width, std::size_t height) {
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		std::ostringstream error;
		error << "Unable to initalize SDL: " << SDL_GetError() << '\n';
		throw std::runtime_error(error.str().c_str());
	}

	sdl_context.window = SDL_CreateWindow("LCGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                                      width, height, SDL_WINDOW_OPENGL);

	if (!sdl_context.window) {
		std::ostringstream error;
		error << "SDL window creation failed: " << SDL_GetError() << '\n';
		throw std::runtime_error(error.str().c_str());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	sdl_context.gl_context = SDL_GL_CreateContext(sdl_context.window);

	SDL_GL_SetSwapInterval(0);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, width, height);
}

void graphics::destroy_renderer() {
	SDL_GL_DeleteContext(sdl_context.gl_context);
	SDL_DestroyWindow(sdl_context.window);
	SDL_Quit();
}
