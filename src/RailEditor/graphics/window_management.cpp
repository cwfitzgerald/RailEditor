#include "graphics/internals.hpp"
#include "utilities/file.hpp"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

graphics::SDL_Context_t graphics::sdl_context;
graphics::OpenGL_Extentions_t graphics::opengl_extentions;

void graphics::initialize_renderer(std::size_t width, std::size_t height) {
	sdl_context.width  = width;
	sdl_context.height = height;
	graphics::initialize_sdl();
	graphics::initialize_ogl();
}

void graphics::initialize_sdl() {
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		std::ostringstream error;
		error << "Unable to initialize SDL: " << SDL_GetError() << '\n';
		throw std::runtime_error(error.str().c_str());
	}

	sdl_context.window =
	    SDL_CreateWindow("LCGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                     int(sdl_context.width), int(sdl_context.height), SDL_WINDOW_OPENGL);

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

	glViewport(0, 0, GLsizei(sdl_context.width), GLsizei(sdl_context.height));

	// Get extention list and version
	GLint n = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	std::vector<const char*> extentions;
	extentions.reserve(n);

	for (GLint i = 0; i < n; ++i) {
		extentions.push_back(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
	}

	auto tess_it = std::find_if(extentions.begin(), extentions.end(), [](const char* val) {
		return std::strcmp(val, "GL_ARB_tessellation_shader");
	});
	if (tess_it != extentions.end()) {
		opengl_extentions.tesselation = true;
	}

	std::cerr << "OpenGL Extentions:\n";
	for (const char* e : extentions) {
		std::cerr << e << '\n';
	}

	auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	if (version != nullptr) {
		std::cerr << "\nOpenGL Version: " << version << '\n';
	}
}

void graphics::initialize_ogl() {
	auto vertshadersrc = utilities::load_file_contents("shaders/test.vs.glsl");
	auto fragshadersrc = utilities::load_file_contents("shaders/test.fs.glsl");

	graphics::Shader<graphics::ShaderType::vertex> vert(vertshadersrc.get());
	graphics::Shader<graphics::ShaderType::fragment> frag(fragshadersrc.get());

	auto prog = create_shader_program(vert, frag);
}

void graphics::destroy_renderer() {
	SDL_GL_DeleteContext(sdl_context.gl_context);
	SDL_DestroyWindow(sdl_context.window);
	SDL_Quit();
}
