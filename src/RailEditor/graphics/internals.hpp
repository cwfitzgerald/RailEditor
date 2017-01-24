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
		union {
			GLuint array[2];
			struct {
				GLuint gBuffer;
				GLuint lBuffer;
			};
		} framebuffers;
		union {
			GLuint array[5];
			struct {
				GLuint gBufferDiffuseSpec;
				GLuint gBufferNormal;
				GLuint gBufferDepth;
				GLuint lBufferColor;
				GLuint lBufferDepth;
			};
		} textures;
	};

	extern OpenGL_Extentions_t opengl_extentions; // defined in ogl_management.cpp
	extern OpenGLData_t opengldata;               // defined in ogl_management.cpp

	void initialize_ogl();
	void check_framebuffer();
	void create_framebuffers();
	void destroy_framebuffers();
	void destroy_ogl();
}
