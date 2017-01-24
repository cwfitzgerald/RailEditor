#include "internals.hpp"

#include <SDL2/SDL.h>
#include <utility>

std::unordered_map<SDL_Keycode, bool> resystem::keydown;
resystem::KeysPressed_t resystem::keypressed;

resystem::EventTypes_t resystem::process_events() {
	static float last_frame_ticks = 0;
	static float frame_ticks = 0;

	static float mouse_x = 0;
	static float mouse_y = 0;

	static bool mouse_left_down = false;
	static bool mouse_right_down = false;

	EventTypes_t et;
	SDL_Event event;

	keypressed.clear();

	// Handle various SDL events
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				et.quit = true;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					et.resize = true;
					SDL_GetWindowSize(sdl_context.window, &et.resize_width, &et.resize_height);
				}
				break;
			case SDL_KEYDOWN:
				keydown[event.key.keysym.sym] = true;
				keypressed.set(event.key.keysym.sym);
				break;
			case SDL_KEYUP:
				keydown[event.key.keysym.sym] = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					mouse_left_down = true;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT) {
					mouse_right_down = true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					mouse_left_down = false;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT) {
					mouse_right_down = false;
				}
				break;
			case SDL_MOUSEWHEEL:
				et.mouse_scroll = event.wheel.y;
				if (event.wheel.direction == SDL_MOUSEWHEEL_NORMAL) {
					et.mouse_scroll *= -1;
				}
				break;
		}
	}
	et.mouseLeftDown = mouse_left_down;
	et.mouseRightDown = mouse_right_down;
	et.keypressed = keypressed.keys.size() != 0;
	keypressed.sort();

	// Handle time
	last_frame_ticks = frame_ticks;
	frame_ticks = SDL_GetTicks() / 1000.0f;
	et.time_elapsed = frame_ticks - last_frame_ticks;

	// Handle mouse movement
	int mouse_pix_x, mouse_pix_y;
	SDL_GetRelativeMouseState(&mouse_pix_x, &mouse_pix_y);

	et.mouse_delta_x = mouse_pix_x / static_cast<float>(sdl_context.width);
	et.mouse_delta_y = mouse_pix_y / static_cast<float>(sdl_context.height);

	mouse_x += et.mouse_delta_x;
	mouse_y += et.mouse_delta_y;

	et.mouseX = mouse_x;
	et.mouseY = mouse_y;

	return et;
}
