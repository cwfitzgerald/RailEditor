#include "graphics/internals.hpp"

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

#ifdef RAIL_EDITOR_DEBUG
#ifdef _MSC_VER
void __stdcall
#else
void
#endif
    ogl_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei len, const GLchar* message,
                 const void* userParam) {
	static std::size_t error_num = 0;

	(void) source;
	(void) len;
	(void) userParam;

	std::cerr << "\n OPENGL CALLBACK #" << error_num << "\n";
	std::cerr << "message: " << message << '\n';
	std::cerr << "type: ";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			std::cerr << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cerr << "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cerr << "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cerr << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cerr << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cerr << "OTHER";
			break;
	}
	std::cerr << '\n';

	std::cerr << "id: " << id << '\n';
	std::cerr << "severity: ";
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:
			std::cerr << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cerr << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			std::cerr << "HIGH";
			break;
	}
	std::cerr << "\n\n";
}
#endif

void graphics::initialize_renderer(std::size_t width, std::size_t height) {
	sdl_context.width = width;
	sdl_context.height = height;
	initialize_sdl();
	initialize_ogl();
}

void graphics::initialize_sdl() {
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		std::ostringstream error;
		error << "Unable to initialize SDL: " << SDL_GetError() << '\n';
		throw std::runtime_error(error.str().c_str());
	}

	sdl_context.window = SDL_CreateWindow("LCGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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
#ifdef RAIL_EDITOR_DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	sdl_context.gl_context = SDL_GL_CreateContext(sdl_context.window);

	SDL_GL_SetSwapInterval(0);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, GLsizei(sdl_context.width), GLsizei(sdl_context.height));

// Register Debug Callback
#ifdef RAIL_EDITOR_DEBUG
	if (glDebugMessageCallback) {
		std::cerr << "Registering OpenGL Callback\n";
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(ogl_callback, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	}
	else {
		std::cerr << "glDebugMessageCallback not available\n";
	}
#endif

	// Get extention list and version
	GLint n = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	std::vector<const char*> extentions;
	extentions.reserve(n);

	for (GLint i = 0; i < n; ++i) {
		extentions.push_back(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
	}

	auto tess_it = std::find_if(extentions.begin(), extentions.end(),
	                            [](const char* val) { return std::strcmp(val, "GL_ARB_tessellation_shader"); });
	if (tess_it != extentions.end()) {
		opengl_extentions.tesselation = true;
	}

	/*std::cerr << "OpenGL Extentions:\n";
	for (const char* e : extentions) {
	    std::cerr << e << '\n';
	}*/

	auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	if (version != nullptr) {
		std::cerr << "\nOpenGL Version: " << version << '\n';
	}
}

void graphics::destroy_sdl() {
	SDL_GL_DeleteContext(sdl_context.gl_context);
	SDL_DestroyWindow(sdl_context.window);
	SDL_Quit();
}

void graphics::destroy_renderer() {
	destroy_ogl();
	destroy_sdl();
}
