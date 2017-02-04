#pragma once

namespace utilities {
	template <class C1, class C2, class C3>
	inline auto clamp(C1 value, C2 min, C3 max) {
		return value < min ? min : (value > max ? max : value);
	}

	inline float lerp(float a0, float a1, float x) {
		float a = (1 - x) * a0;
		float b = x * a1;
		float res = a + b;
		return res;
	}

	inline float lerp2d(float x, float y, float a0, float a1, float b0, float b1) {
		return lerp(lerp(a0, a1, x), lerp(b0, b1, x), y);
	}
}
