#include "graphics.hpp"
#include "system.hpp"
#include "world.hpp"
#include <iostream>

int main(int, char**) {
	std::cout << "Hello ChickenShit!\n";

	graphics::initialize_renderer(1280, 720);

	auto surf = world::Surface(100, 100);
	surf.upload();
	auto hm = world::Heightmap(500, 500);
	hm.upload();
	auto camera = world::Camera(hm);

	bool game_running = true;
	bool has_mouse = true;

	while (game_running) {
		auto events = resystem::process_events();

#ifdef RAIL_EDITOR_DEBUG
// std::cerr << "MouseRight Down: " << events.mouseRightDown << ", DeltaX: " << events.mouse_delta_x
//           << ", Delta Y: " << events.mouse_delta_y << '\n';
#endif
		if (events.quit) {
			game_running = false;
		}
		camera.zoom(events.mouse_scroll);
		if (events.mouseRightDown) {
			if (resystem::keydown[SDLK_LSHIFT]) {
				camera.slide(events.mouse_delta_y, events.mouse_delta_x);
			}
			else {
				camera.rotate(events.mouse_delta_y * 100, events.mouse_delta_x * 100);
			}
		}
		if (events.keypressed) {
			if (resystem::keypressed.pressed(SDLK_ESCAPE)) {
				game_running = false;
			}
			if (resystem::keypressed.pressed(SDLK_LALT)) {
				if (has_mouse) {
					resystem::release_mouse();
				}
				else {
					resystem::grab_mouse();
				}
				has_mouse = !has_mouse;
			}
		}

		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		graphics::set_camera_mat(camera.matrix());

		hm.bind();
		surf.render();

		graphics::render();
	}

	graphics::destroy_renderer();
}
