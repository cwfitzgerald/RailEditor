#include "graphics.hpp"
#include "world.hpp"
#include <iostream>

int main(int, char**) {
	std::cout << "Hello ChickenShit!\n";

	graphics::initialize_renderer(1280, 720);
	
	auto surf = world::Surface(3, 3);
	surf.upload();
	auto hm = world::Heightmap(50, 50);
	hm.upload();

	for (std::size_t frames = 0; frames < 10000; ++frames){
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		hm.bind();
		surf.render();

		graphics::render();
	}

	graphics::destroy_renderer();
}
