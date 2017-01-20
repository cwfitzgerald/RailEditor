#pragma once

#include "graphics.hpp"

#include <cstring>
#include <vector>

namespace world {
	class Heightmap {
	  private:
		std::vector<float> vals;
		std::size_t width;
		std::size_t height;

		void create_heights(std::size_t seed);

	  public:
		Heightmap() : vals(), width(0), height(0){};
		Heightmap(const Heightmap&) = default;
		Heightmap(Heightmap&&) = default;
		Heightmap& operator=(const Heightmap&) = default;
		Heightmap& operator=(Heightmap&&) = default;

		Heightmap(std::size_t width, std::size_t height, std::size_t seed = 0ULL)
		    : vals(width * height, 0.0f), width(width), height(height) {
			create_heights(seed);
		}

		void regenerate(std::size_t width, std::size_t height, std::size_t seed = 0ULL) {
			vals.resize(width * height, 0.0f);
			std::memset(vals.data(), 0, width * height);
			this->width = width;
			this->height = height;

			create_heights(seed);
		}
	};

	class Surface {
	  private:
		std::vector<graphics::Vertex> vertices;
		std::vector<uint32_t> indices;

	  public:
		Surface() = default;
		Surface(const Surface&) = default;
		Surface(Surface&&) = default;
		Surface& operator=(const Surface&) = default;
		Surface& operator=(Surface&&) = default;

		Surface(std::size_t width, std::size_t height) {
			regenerate(width, height);
		}

		void regenerate(std::size_t width, std::size_t height);
		void upload_data();
	};
}