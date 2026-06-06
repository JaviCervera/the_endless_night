#pragma once

#include <algorithm>
#include <cstdint>
#include "real.h"

#define COLOR_RED 0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE 0xFF0000FF
#define COLOR_CYAN 0x00FFFF
#define COLOR_MAGENTA 0xFFFF00FF
#define COLOR_YELLOW 0xFFFFFF00
#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GRAY 0xFF808080
#define COLOR_LIGHTGRAY 0xFFBFBFBF
#define COLOR_DARKGRAY 0xFF404040
#define COLOR_ORANGE 0xFFFFA500
#define COLOR_BROWN 0xFF8B4513

using color_t = uint32_t;
using colorcomp_t = uint8_t;

inline color_t rgb(colorcomp_t r, colorcomp_t g, colorcomp_t b, colorcomp_t a = colorcomp_t(255)) noexcept
{
	r = std::clamp<colorcomp_t>(r, 0, 255);
	g = std::clamp<colorcomp_t>(g, 0, 255);
	b = std::clamp<colorcomp_t>(b, 0, 255);
	a = std::clamp<colorcomp_t>(a, 0, 255);
	return (a << 24) | (r << 16) | (g << 8) | b;
}

inline color_t rgbf(real_t r, real_t g, real_t b, real_t a = real_t(1)) noexcept
{
	return rgb(colorcomp_t(int(r * 255)), colorcomp_t(int(g * 255)), colorcomp_t(int(b * 255)), colorcomp_t(int(a * 255)));
}

inline colorcomp_t color_red(color_t color) noexcept
{
	return (color >> 16) & 0xff;
}

inline colorcomp_t color_green(color_t color) noexcept
{
	return (color >> 8) & 0xff;
}

inline colorcomp_t color_blue(color_t color) noexcept
{
	return color & 0xff;
}

inline colorcomp_t color_alpha(color_t color) noexcept
{
	return colorcomp_t((color >> 24) & 0xff);
}

inline real_t color_redf(color_t color) noexcept
{
	return real_t(color_red(color)) / 255;
}

inline real_t color_greenf(color_t color) noexcept
{
	return real_t(color_green(color)) / 255;
}

inline real_t color_bluef(color_t color) noexcept
{
	return real_t(color_blue(color)) / 255;
}

inline real_t color_alphaf(color_t color) noexcept
{
	return real_t(color_alpha(color)) / 255;
}

inline color_t color_fade(color_t color, colorcomp_t new_alpha) noexcept
{
	return (std::clamp<colorcomp_t>(new_alpha, 0, 255) << 24) | (color & 0x00ffffff);
}

inline color_t color_mul(color_t color, real_t factor) noexcept
{
	return rgbf(color_redf(color) * factor, color_greenf(color) * factor, color_bluef(color) * factor, color_alphaf(color));
}

inline color_t color_swap(color_t color) noexcept
{
	return rgb(color_blue(color), color_green(color), color_red(color), color_alpha(color));
}
