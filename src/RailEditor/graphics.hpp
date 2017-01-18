#pragma once

#include <cinttypes>
#include <cstdlib>

namespace graphics {
	void initialize_renderer(std::size_t width, std::size_t height);
	void resize(std::size_t width, std::size_t height);
	void destroy_renderer();
}