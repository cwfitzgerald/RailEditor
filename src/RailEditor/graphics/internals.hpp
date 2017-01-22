#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "graphics.hpp"
#include "shader.hpp"

namespace graphics {
	struct OpenGL_Extentions_t {
		bool tesselation = false;
	};

	struct OpenGLData_t {
		ShaderProgram testshader;
	};

	extern OpenGL_Extentions_t opengl_extentions; // defined in ogl_management.cpp
	extern OpenGLData_t opengldata;               // defined in ogl_management.cpp

	void initialize_ogl();
	void destroy_ogl();
}
