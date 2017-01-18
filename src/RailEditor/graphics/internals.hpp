#pragma once

#include "graphics.hpp"
#include "shaders.hpp"

namespace graphics {
	void initialize_sdl(std::size_t width, std::size_t height);
	void initialize_shaders();
}
