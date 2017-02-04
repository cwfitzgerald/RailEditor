#include "internals.hpp"
#include "system/internals.hpp"
#include "utilities/file.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <type_traits>
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

	auto tess_it = std::find_if(extentions.begin(), extentions.end(), [](const char* val) {
		return std::strcmp(val, "GL_ARB_tessellation_shader");
	});
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
	auto m_mat_u = opengldata.testshader.get_uniform("m_mat");
	// auto v_mat_u = opengldata.testshader.get_uniform("v_mat");
	auto p_mat_u = opengldata.testshader.get_uniform("p_mat");
	auto tex_u = opengldata.testshader.get_uniform("heightmap");

	glm::mat4 model{};
	glm::mat4 projection{};
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3{10, 10, 1});
	projection =
	    glm::perspective(glm::radians(80.0f), static_cast<float>(resystem::sdl_context.width) /
	                                              static_cast<float>(resystem::sdl_context.height),
	                     0.1f, 100.0f);

	opengldata.world_model = model;
	opengldata.projection = projection;

	glUniformMatrix4fv(m_mat_u, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(p_mat_u, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(tex_u, 0);

	auto pass_vert_shader_source = utilities::load_file_contents("shaders/passthrough.vs.glsl");
	auto pass_frag_shader_source = utilities::load_file_contents("shaders/passthrough.fs.glsl");

	graphics::Shader<graphics::ShaderType::vertex> pass_vert(pass_vert_shader_source.get());
	graphics::Shader<graphics::ShaderType::fragment> pass_frag(pass_frag_shader_source.get());

	opengldata.passthroughshader = create_shader_program(pass_vert, pass_frag);
	opengldata.passthroughshader.use();

	auto AlbedoSpec_u = opengldata.passthroughshader.get_uniform("AlbedoSpec");
	auto Normal_u = opengldata.passthroughshader.get_uniform("Normal");

	glUniform1i(AlbedoSpec_u, 0);
	glUniform1i(Normal_u, 1);

	create_framebuffers();
}

void graphics::set_camera_mat(const glm::mat4& mat) {
	opengldata.normal = glm::mat3(glm::transpose(glm::inverse(mat * opengldata.world_model)));

	auto v_mat_u = opengldata.testshader.get_uniform("v_mat");
	auto normal_mat_u = opengldata.testshader.get_uniform("normal_mat");
	opengldata.testshader.use();
	glUniformMatrix4fv(v_mat_u, 1, GL_FALSE, glm::value_ptr(mat));
	glUniformMatrix3fv(normal_mat_u, 1, GL_FALSE, glm::value_ptr(opengldata.normal));
}

void graphics::resize(std::size_t width, std::size_t height) {
	resystem::sdl_context.width = width;
	resystem::sdl_context.height = height;

	destroy_framebuffers();
	create_framebuffers();

	glViewport(0, 0, width, height);
}

void graphics::check_framebuffer() {
	auto fberr = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fberr != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!\n";
		switch (fberr) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				std::cerr << "Framebuffer incomplete attachment\n";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				std::cerr << "Framebuffer incomplete dimentions\n";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				std::cerr << "Framebuffer missing attachment\n";
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				std::cerr << "Framebuffer unsupported\n";
				break;
		}
		throw std::runtime_error("Framebuffer incomplete");
	}
}

void graphics::create_framebuffers() {
	glGenFramebuffers(std::extent<decltype(opengldata.framebuffers.array)>::value,
	                  opengldata.framebuffers.array);
	glGenTextures(std::extent<decltype(opengldata.textures.array)>::value,
	              opengldata.textures.array);

	float x = resystem::sdl_context.width;
	float y = resystem::sdl_context.height;

	// Deal with gBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, opengldata.framebuffers.gBuffer);

	// gBuffer - Diffuse Color and Specular Intensity
	glBindTexture(GL_TEXTURE_2D, opengldata.textures.gBufferAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
	                       opengldata.textures.gBufferAlbedoSpec, 0);

	// gBuffer - Normals
	glBindTexture(GL_TEXTURE_2D, opengldata.textures.gBufferNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
	                       opengldata.textures.gBufferNormal, 0);

	// gBuffer - Depth
	glBindTexture(GL_TEXTURE_2D, opengldata.textures.gBufferDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, x, y, 0, GL_DEPTH_STENCIL,
	             GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
	                       opengldata.textures.gBufferDepth, 0);

	constexpr GLenum attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(std::extent<decltype(attachments)>::value, attachments);

	check_framebuffer();

	// Deal with lBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, opengldata.framebuffers.lBuffer);

	// lBuffer - Color
	glBindTexture(GL_TEXTURE_2D, opengldata.textures.lBufferColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
	                       opengldata.textures.lBufferColor, 0);

	// lBuffer - Depth
	glBindTexture(GL_TEXTURE_2D, opengldata.textures.lBufferDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, x, y, 0, GL_DEPTH_STENCIL,
	             GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
	                       opengldata.textures.lBufferDepth, 0);

	check_framebuffer();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void graphics::destroy_framebuffers() {
	glDeleteTextures(std::extent<decltype(opengldata.textures), 0>::value,
	                 opengldata.textures.array);
	glDeleteFramebuffers(std::extent<decltype(opengldata.framebuffers), 0>::value,
	                     opengldata.framebuffers.array);
}

void graphics::destroy_ogl() {
	opengldata.testshader.clear();
	destroy_framebuffers();
}

void graphics::destroy_renderer() {
	destroy_ogl();
	resystem::destroy_sdl();
}
