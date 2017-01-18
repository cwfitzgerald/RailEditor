#pragma once

#include <GL/glew.h>

#include <string>
#include <sstream>
#include <memory>
#include <type_traits>

namespace graphics{
	enum class ShaderType {
		null = 0,
		vertex = GL_VERTEX_SHADER,
		tesscontrol = GL_TESS_CONTROL_SHADER,
		tesseval = GL_TESS_EVALUATION_SHADER,
		geometry = GL_GEOMETRY_SHADER,
		fragment = GL_FRAGMENT_SHADER,
		compute = GL_COMPUTE_SHADER
	};

	template<ShaderType stype = ShaderType::null>
	class Shader {
	  private:
		GLuint compiled;
		std::string src;
	  public:
		constexpr static ShaderType type = stype;

		Shader() = default;
		Shader(const char * src_ptr) : src(src_ptr){};
		Shader(const Shader&) = default;
		Shader(Shader&&) = default;
		Shader& operator=(const Shader&) = default;
		Shader& operator=(Shader&&) = default;

		~Shader() {
			clear();
		}

		void set_src(const std::string& isrc) {
			src = isrc;
		}

		std::string get_src() {
			return src;
		}

		GLuint get_gl() {
			return compiled;
		}

		GLuint compile() {
			if(compiled == 0) {
				compiled = glCreateShader(static_cast<typename std::underlying_type<ShaderType>::type>(type));
			}

			const char * ptr = src.c_str();
			glShaderSource(compiled, 1, &ptr, nullptr);
			glCompileShader(compiled);

			GLint success = 0;
			glGetShaderiv(compiled, GL_COMPILE_STATUS, &success);

			if (success == GL_FALSE) {
				GLint logSize = 0;
				glGetShaderiv(compiled, GL_INFO_LOG_LENGTH, &logSize);

				std::unique_ptr<char[]> err_log(new char[logSize]);

				GLsizei len;
				glGetShaderInfoLog(compiled, logSize, &len, err_log.get());

				err_log[logSize - 1] = '\0';

				std::ostringstream err;
				err << "Shader compilation failed.\n" << err_log.get() << '\n';

				throw std::runtime_error(err.str().c_str());
			}

			return compiled;
		}

		void clear() {
			glDeleteShader(compiled);
		}
	};
}
