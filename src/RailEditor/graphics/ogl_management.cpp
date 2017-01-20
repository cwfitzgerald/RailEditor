#include "internals.hpp"
#include "utilities/file.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

graphics::OpenGLData_t graphics::opengldata;

void graphics::initialize_ogl() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	auto vertshadersrc = utilities::load_file_contents("shaders/test.vs.glsl");
	auto fragshadersrc = utilities::load_file_contents("shaders/test.fs.glsl");

	graphics::Shader<graphics::ShaderType::vertex> vert(vertshadersrc.get());
	graphics::Shader<graphics::ShaderType::fragment> frag(fragshadersrc.get());

	opengldata.testshader = create_shader_program(vert, frag);
	opengldata.testshader.use();
	auto pvm_mat_u = opengldata.testshader.get_uniform("pvm_mat");
	auto tex_u     = opengldata.testshader.get_uniform("heightmap");

	glm::mat4 transform{};
	transform = glm::scale(transform, glm::vec3{10, 10, 1});
	transform = glm::lookAt(glm::vec3{0, 1, 4}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}) * transform;
	transform = glm::perspective(glm::radians(80.0f), static_cast<float>(sdl_context.width) /
	                                                      static_cast<float>(sdl_context.height),
	                             0.1f, 100.0f) *
	            transform;

	glUniformMatrix4fv(pvm_mat_u, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1i(tex_u, 0);
}

void graphics::render() {
	SDL_GL_SwapWindow(sdl_context.window);
}

void graphics::destroy_ogl() {
	opengldata.testshader.clear();
}
