#include "graphics.hpp"
#include "world.hpp"
#include <iostream>

int main(int, char**) {
	std::cout << "Hello ChickenShit!\n";

	graphics::initialize_renderer(1280, 720);
	graphics::destroy_renderer();
}
