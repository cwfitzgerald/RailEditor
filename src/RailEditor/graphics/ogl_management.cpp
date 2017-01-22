#include "internals.hpp"
#include "system/internals.hpp"
#include "utilities/file.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <vector>

graphics::OpenGLData_t graphics::opengldata;
graphics::OpenGL_Extentions_t graphics::opengl_extentions;

void graphics::initialize_renderer(std::size_t width, std::size_t height) {
	resystem::sdl_context.width = width;
	resystem::sdl_context.height = height;
	resystem::initialize_sdl();
	initialize_ogl();
}

void graphics::initialize_ogl() {
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

	// Other initalization stuff

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
	auto tex_u = opengldata.testshader.get_uniform("heightmap");

	glm::mat4 transform{};
	transform = glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	transform = glm::scale(transform, glm::vec3{1, 1, 1});
	transform = glm::lookAt(glm::vec3{2, 1, 3}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}) * transform;
	transform = glm::perspective(glm::radians(80.0f), static_cast<float>(resystem::sdl_context.width) /
	                                                      static_cast<float>(resystem::sdl_context.height),
	                             0.1f, 100.0f) *
	            transform;

	glUniformMatrix4fv(pvm_mat_u, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1i(tex_u, 0);
}

void graphics::render() {
	SDL_GL_SwapWindow(resystem::sdl_context.window);
}

void graphics::destroy_ogl() {
	opengldata.testshader.clear();
}

void graphics::destroy_renderer() {
	destroy_ogl();
	resystem::destroy_sdl();
}
