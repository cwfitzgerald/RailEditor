#pragma once

#include <SDL2/sdl.h>

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace resystem {
	struct EventTypes_t {
		bool keypressed = false;
		bool resize = false;
		bool quit = false;
		int resize_width = 0;
		int resize_height = 0;
		float mouseX = 0;
		float mouseY = 0;
		float mouse_delta_x = 0;
		float mouse_delta_y = 0;
		float time_elapsed = 0;
	};

	class KeysPressed_t {
	  private:
		std::vector<SDL_Keycode> keys;

		void set(SDL_Keycode k) {
			keys.push_back(k);
		}

		void sort() {
			std::sort(keys.begin(), keys.end());
		}

		void clear() {
			keys.clear();
		}

	  public:
		bool pressed(SDL_Keycode k) {
			auto it = std::lower_bound(keys.begin(), keys.end(), k);
			return it != keys.end();
		}

		friend EventTypes_t process_events();
	};

	extern std::unordered_map<SDL_Keycode, bool> keydown; // defined in event_processing.cpp
	extern KeysPressed_t keypressed;                      // defined in event_processing.cpp

	EventTypes_t process_events();
}
