#pragma once

#include <GL/glew.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "utilities/meta.hpp"

namespace graphics {
	enum class ShaderType : GLenum {
		null = 0,
		vertex = GL_VERTEX_SHADER,
		tesscontrol = GL_TESS_CONTROL_SHADER,
		tesseval = GL_TESS_EVALUATION_SHADER,
		geometry = GL_GEOMETRY_SHADER,
		fragment = GL_FRAGMENT_SHADER,
		compute = GL_COMPUTE_SHADER
	};

	template <ShaderType stype = ShaderType::null>
	class Shader {
	  private:
		GLuint compiled;
		std::string src;
		bool valid;

	  public:
		constexpr static ShaderType type = stype;

		Shader() = default;
		Shader(const char* src_ptr) : compiled(0), src(src_ptr), valid(false){};
		Shader(const Shader&) = delete;
		Shader(Shader&& rhs) {
			compiled = rhs.compiled;
			src = std::move(rhs.compiled);
			valid = rhs.valid;
			rhs.compiled = 0;
		};
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&& rhs) {
			compiled = rhs.compiled;
			src = std::move(rhs.compiled);
			valid = rhs.valid;
			rhs.compiled = 0;

			return *this;
		};

		~Shader() {
			clear();
		}

		void set_src(const std::string& isrc) {
			src = isrc;
			valid = false;
		}

		std::string get_src() {
			return src;
		}

		GLuint get_gl() {
			return compiled;
		}

		GLuint compile() {
			if (valid) {
				return compiled;
			}
			if (compiled == 0) {
				compiled = glCreateShader(static_cast<typename std::underlying_type<ShaderType>::type>(type));
			}

			const char* ptr = src.c_str();
			glShaderSource(compiled, 1, &ptr, nullptr);
			glCompileShader(compiled);

			GLint success = 0;
			glGetShaderiv(compiled, GL_COMPILE_STATUS, &success);

			if (success == GL_FALSE) {
				GLint logSize = 0;
				glGetShaderiv(compiled, GL_INFO_LOG_LENGTH, &logSize);

				std::unique_ptr<char[]> err_log(new char[logSize + 1]);

				GLsizei len;
				glGetShaderInfoLog(compiled, logSize, &len, err_log.get());

				err_log[logSize] = '\0';

				std::ostringstream err;
				err << "Shader compilation failed.\n" << err_log.get() << '\n';

				std::cerr << err.str();

				throw std::runtime_error(err.str().c_str());
			}

			valid = true;
			return compiled;
		}

		void clear() {
			glDeleteShader(compiled);
			compiled = 0;
			valid = false;
		}
	};

	class ShaderProgram {
	  private:
		std::unordered_map<std::string, GLuint> uniforms;
		GLuint program;

	  public:
		ShaderProgram() : program(0){};
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&& rhs) {
			program = rhs.program;
			rhs.program = 0;
		}
		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&& rhs) {
			program = rhs.program;
			rhs.program = 0;

			return *this;
		}

		GLuint get_uniform(const std::string& name) {
			auto it = uniforms.find(name);
			if (it == uniforms.end()) {
				it = uniforms.insert({name, glGetUniformLocation(program, name.c_str())}).first;
			}
			return it->second;
		}

		void use() {
			glUseProgram(program);
		}

		void clear() {
			glDeleteProgram(program);
			program = 0;
		}

		~ShaderProgram() {
			clear();
		}

		template <class... Shader_Types>
		friend ShaderProgram create_shader_program(ShaderProgram& prog, Shader_Types&&... t);
	};

	template <class Shader1>
	void attach_shaders(GLuint program, Shader1&& a) {
		glAttachShader(program, a.compile());
	}

	template <class Shader1, class Shader2, class... Shader_Types>
	void attach_shaders(GLuint program, Shader1&& a, Shader2&& b, Shader_Types&&... s) {
		glAttachShader(program, a.compile());
		attach_shaders(program, b, s...);
	}

	template <class Shader1>
	void detach_shaders(GLuint program, Shader1&& a) {
		glDetachShader(program, a.get_gl());
	}

	template <class Shader1, class Shader2, class... Shader_Types>
	void detach_shaders(GLuint program, Shader1&& a, Shader2&& b, Shader_Types&&... s) {
		glDetachShader(program, a.get_gl());
		detach_shaders(program, b, s...);
	}

	template <class... Shader_Types,
	          typename std::enable_if_t<utilities::has_none<ShaderProgram, Shader_Types...>::value, void>* = nullptr>
	ShaderProgram create_shader_program(Shader_Types&&... s) {
		auto prog = ShaderProgram{};
		return create_shader_program(prog, s...);
	}

	template <class... Shader_Types>
	ShaderProgram create_shader_program(ShaderProgram& prog, Shader_Types&&... s) {
		static_assert(utilities::has_one<Shader<ShaderType::vertex>, Shader_Types...>::value,
		              "Shader program must have one vertex shader");
		static_assert(utilities::has_one<Shader<ShaderType::fragment>, Shader_Types...>::value,
		              "Shader program must have one fragment shader");
		static_assert(utilities::has_no_more_than_one<Shader<ShaderType::tesscontrol>, Shader_Types...>::value,
		              "Shader program can't have more than one tesselation control shader");
		static_assert(utilities::has_no_more_than_one<Shader<ShaderType::tesseval>, Shader_Types...>::value,
		              "Shader program can't have more than one tesselation evaluation shader");
		static_assert(utilities::has_no_more_than_one<Shader<ShaderType::geometry>, Shader_Types...>::value,
		              "Shader program can't have more than one geometry shader");
		static_assert(utilities::has_none<Shader<ShaderType::compute>, Shader_Types...>::value,
		              "Shading program can't have compute shaders");

		if (prog.program == 0) {
			prog.program = glCreateProgram();
		}

		attach_shaders(prog.program, s...);

		glLinkProgram(prog.program);

		GLint isLinked = 0;
		glGetProgramiv(prog.program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE) {
			GLint logSize = 0;
			glGetProgramiv(prog.program, GL_INFO_LOG_LENGTH, &logSize);

			std::unique_ptr<char[]> err_log(new char[logSize + 1]);

			GLsizei len;
			glGetProgramInfoLog(prog.program, logSize, &len, err_log.get());

			err_log[logSize] = '\0';

			std::ostringstream err;
			err << "Shader linking failed.\n" << err_log.get() << '\n';

			std::cerr << err.str();

			throw std::runtime_error(err.str().c_str());
		}

		detach_shaders(prog.program, s...);

		return std::move(prog);
	}
}
