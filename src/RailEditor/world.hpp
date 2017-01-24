#pragma once

#include "graphics.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <cinttypes>
#include <cstring>
#include <vector>

namespace world {
	// NOTE: Texture data goes from bottom left to top right
	// [240] -> [255]
	//   :        :
	//  [0]  -> [15]

	// (0,1) ... (1,1)
	//   :         :
	// (0,0) ... (1,0)
	class Heightmap {
	  private:
		std::vector<float> vals;
		std::size_t width;
		std::size_t height;
		GLuint gl_tex;

		void create_heights(std::size_t seed);

	  public:
		Heightmap() : vals(), width(0), height(0), gl_tex(0){};
		Heightmap(const Heightmap&) = delete;
		Heightmap(Heightmap&& rhs) {
			*this = std::move(rhs);
		}
		Heightmap& operator=(const Heightmap&) = delete;
		Heightmap& operator=(Heightmap&& rhs) {
			vals = std::move(rhs.vals);
			width = rhs.width;
			height = rhs.height;
			gl_tex = rhs.gl_tex;
			rhs.gl_tex = 0;

			return *this;
		}

		~Heightmap() {
			glDeleteTextures(1, &gl_tex);
		}

		Heightmap(std::size_t width, std::size_t height, std::size_t seed = 0ULL)
		    : vals(width * height, 0.0f), width(width), height(height), gl_tex(0) {
			create_heights(seed);
		}

		float get_value(float x, float y);

		void regenerate(std::size_t width, std::size_t height, std::size_t seed = 0ULL) {
			vals.resize(width * height, 0.0f);
			std::memset(vals.data(), 0, width * height * sizeof(float));
			this->width = width;
			this->height = height;

			create_heights(seed);
		}

		void upload();
		void bind(std::size_t texnum = 0);
	};

	class Surface {
	  private:
		std::vector<graphics::Vertex> vertices;
		std::vector<GLuint> indices;

		GLuint vao = 0;
		GLuint vert_buff = 0, index_buff = 0;

	  public:
		Surface() = default;
		Surface(const Surface&) = delete;
		Surface(Surface&& rhs) {
			*this = std::move(rhs);
		}
		Surface& operator=(const Surface&) = delete;
		Surface& operator=(Surface&& rhs) {
			vertices = std::move(rhs.vertices);
			indices = std::move(rhs.indices);
			vao = rhs.vao;
			vert_buff = rhs.vert_buff;
			index_buff = rhs.index_buff;

			rhs.vao = 0;
			rhs.vert_buff = 0;
			rhs.index_buff = 0;

			return *this;
		}

		~Surface() {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vert_buff);
			glDeleteBuffers(1, &index_buff);
		}

		Surface(std::size_t width, std::size_t height) {
			regenerate(width, height);
		}

		void regenerate(std::size_t width, std::size_t height);
		void upload();
		void render();
	};

	class Camera {
	  private:
		Heightmap& hm;
		glm::mat4 mat;
		glm::vec3 focus_point;
		glm::vec3 cam_point;
		float direction;
		float angle;
		float distance;
		bool ready;

		float get_hightmap_value(float x, float z);

		void update_position();
		void update_matrix();

	  public:
		Camera(Heightmap& hm)
		    : hm(hm),
		      mat{},
		      focus_point{0, 0, 0},
		      cam_point{0, 0, 0},
		      direction{0},
		      angle{-30},
		      distance{2},
		      ready{false} {
			update_position();
		};
		Camera(const Camera&) = default;
		Camera(Camera&&) = default;
		Camera& operator=(const Camera&) = default;
		Camera& operator=(Camera&&) = default;

		void slide(float forward, float right);
		void rotate(float up, float right);
		void zoom(float in);

		glm::mat4 matrix() {
			if (!ready) {
				update_matrix();
			}
			return mat;
		}

		float* matrix_ptr() {
			return glm::value_ptr(mat);
		}
	};
}
