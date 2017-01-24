#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "world.hpp"
#include "utilities/random.hpp"

#include <cstdlib>

void world::Heightmap::create_heights(std::size_t seed) {
	(void) seed;

	std::size_t offset = (height - 1) * width;

	double real_seed = static_cast<double>(seed) * 10.0;

	for (std::size_t y = 0; y < height; ++y) {
		double y_ratio = static_cast<double>(y) / height * 10.0f;
		for (std::size_t x = 0; x < width; ++x) {
			double x_ratio = static_cast<double>(x) / height * 10.0f;
			vals[offset + x] = utilities::perlin_noise(x_ratio, y_ratio, real_seed);
		}

		offset -= width;
	}

	// Code for nice mountain
	// for (std::size_t y = 0, base = (height - 1) * width; y < height; ++y, base -= width) {
	// 	float yfactor = std::sin(static_cast<float>(y) * static_cast<float>(M_PI / height) * 2);
	// 	for (std::size_t x = 0; x < width; ++x) {
	// 		float xfactor = std::sin(static_cast<float>(x) * static_cast<float>(M_PI / width) *
	// 0.5);
	// 		vals[base + x] = xfactor * yfactor;
	// 	}
	// }
}

void world::Surface::regenerate(std::size_t width, std::size_t height) {
	vertices.resize(width * height);

	// Generate vertices

	float x_pos_step = 2.0f / (width - 1);
	float y_pos_step = 2.0f / (height - 1);
	float x_uv_step = 1.0f / (width - 1);
	float y_uv_step = 1.0f / (height - 1);

	float y_pos = -1.0f;
	float y_uv = 0.0f;
	for (std::size_t y = 0; y < height; ++y) {
		float x_pos = -1.0f;
		float x_uv = 0.0f;
		for (std::size_t x = 0; x < width; ++x) {
			auto&& vert = vertices[y * width + x];

			vert.location = glm::vec3(x_pos, y_pos, 0.0f);
			vert.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			vert.texcoord = glm::vec2(x_uv, y_uv);

			x_pos += x_pos_step;
			x_uv += x_uv_step;
		}

		y_pos += y_pos_step;
		y_uv += y_uv_step;
	}

	// Generate indices

	std::size_t indices_count = ((width - 1) * 6) * (height - 1);
	indices.resize(indices_count, 0);

	std::size_t index_width = width - 1;
	std::size_t index_height = height - 1;

	for (std::size_t y = 0; y < index_height; ++y) {
		for (std::size_t x = 0; x < index_width; ++x) {
			std::size_t base = (y * index_width + x) * 6;

			indices[base + 0] = (y + 0) * width + (x + 0); // T1 : Top left
			indices[base + 1] = (y + 0) * width + (x + 1); // T1 : Top right
			indices[base + 2] = (y + 1) * width + (x + 0); // T1 : Bottom left
			indices[base + 3] = indices[base + 2];         // T2 : Bottom left
			indices[base + 4] = indices[base + 1];         // T2 : Top right
			indices[base + 5] = (y + 1) * width + (x + 1); // T2 : Bottom right
		}
	}
}
