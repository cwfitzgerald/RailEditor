#include "internals.hpp"
#include <system/internals.hpp>

#include <GL/glew.h>

void graphics::prepare() {
	glBindFramebuffer(GL_FRAMEBUFFER, opengldata.framebuffers.gBuffer);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	opengldata.testshader.use();
}

void graphics::render() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	opengldata.passthroughshader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opengldata.textures.gBufferAlbedoSpec);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, opengldata.textures.gBufferNormal);

	render_fullscreen_quad();

	SDL_GL_SwapWindow(resystem::sdl_context.window);
}

void graphics::render_fullscreen_quad() {
	static GLuint quadVAO = 0;
	static GLuint quadVBO;

	if (quadVAO == 0) {
		constexpr GLfloat quadVertices[] = {
		    // Positions         // Texture Coords
		    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
		    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
		    1.0f,  1.0f,  0.0f, 1.0f, 1.0f, //
		    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
		                      (GLvoid*) (3 * sizeof(GLfloat)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
