#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "graphics.hpp"
#include "shader.hpp"

namespace graphics {
	struct OpenGL_Extentions_t {
		bool tesselation = false;
	};

	struct OpenGLData_t {
		glm::mat4 world_model;
		glm::mat4 view;
		glm::mat3 normal;
		glm::mat4 projection;
		ShaderProgram testshader;
		ShaderProgram passthroughshader;
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
				GLuint gBufferAlbedoSpec;
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

	void render_fullscreen_quad();
}
