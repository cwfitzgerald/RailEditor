#pragma once

#include <cinttypes>
#include <cstdlib>

#include <glm/glm.hpp>

namespace graphics {
	void initialize_renderer(std::size_t width, std::size_t height);
	void resize(std::size_t width, std::size_t height);
	void render();
	void destroy_renderer();

	struct Vertex {
		glm::vec3 location;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};

	static_assert(sizeof(Vertex) == (3 + 3 + 2) * sizeof(float), "Padding added");
}
