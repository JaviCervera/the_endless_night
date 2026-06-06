#pragma once

// Rectangular sub-region of a backbuffer to render into.
// w==0 or h==0 is treated as "full backbuffer" by renderers.
struct viewport_t
{
	int x = 0, y = 0, w = 0, h = 0;
	constexpr viewport_t() = default;
	constexpr viewport_t(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
};
