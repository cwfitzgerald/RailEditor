#pragma once

#include "graphics.hpp"

#include <GL/glew.h>
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
		std::vector<GLushort> indices;

		GLuint vao = 0;
		GLuint vert_buff = 0, index_buff = 0;

	public:
		Surface() = default;
		Surface(const Surface&) = delete;
		Surface(Surface&& rhs) {
			*this = std::move(rhs);
		}
		Surface& operator=(const Surface&) = delete;
		Surface& operator=(Surface&& rhs){
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
}
