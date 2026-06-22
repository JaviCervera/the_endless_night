#include <climits>
#include <fstream>
#include <allegro.h>
#include "pal.h"

static std::array<uint8_t, 256 * 3> _pal;

static int _fade_from[3] = {100, 100, 100};
static int _fade_to[3] = {100, 100, 100};
static int _fade_cur[3] = {100, 100, 100};
static int _fade_step = 0;
static int _fade_steps = 0;
static bool _fade_active = false;

bool pal_load(const char *filename)
{
	std::array<char, 3> id;
	int hex;
	uint8_t ver;
	auto input = std::ifstream{filename, std::ios::binary};
	if (!input)
	{
		return false;
	}
	input.read(id.data(), sizeof(id));
	input.read(reinterpret_cast<char *>(&hex), sizeof(hex));
	input.read(reinterpret_cast<char *>(&ver), sizeof(ver));
	input.read(reinterpret_cast<char *>(_pal.data()), _pal.size());
	PALETTE palette;
	for (auto i = 0; i < 256; ++i)
	{
		palette[i].r = _pal[i * 3];
		palette[i].g = _pal[i * 3 + 1];
		palette[i].b = _pal[i * 3 + 2];
	}
	set_palette(palette);
	return true;
}

color_t pal_rgb(palcolor_t color_index)
{
	return rgb(_pal[color_index * 3] * 4, _pal[color_index * 3 + 1] * 4, _pal[color_index * 3 + 2] * 4);
}

palcolor_t pal_find_closest(uint8_t r, uint8_t g, uint8_t b)
{
	int best_idx = 0;
	int best_dist = INT_MAX;
	for (int i = 0; i < 256; ++i)
	{
		const int dr = int(r) - int(_pal[i * 3] * 4);
		const int dg = int(g) - int(_pal[i * 3 + 1] * 4);
		const int db = int(b) - int(_pal[i * 3 + 2] * 4);
		const int dist = dr * dr + dg * dg + db * db;
		if (dist < best_dist)
		{
			best_dist = dist;
			best_idx = i;
		}
	}
	return palcolor_t(best_idx);
}

void pal_set_fade(int r_pct, int g_pct, int b_pct)
{
	_fade_cur[0] = r_pct;
	_fade_cur[1] = g_pct;
	_fade_cur[2] = b_pct;

	PALETTE palette;
	for (int i = 0; i < 256; ++i)
	{
		const uint8_t r = _pal[i * 3];
		const uint8_t g = _pal[i * 3 + 1];
		const uint8_t b = _pal[i * 3 + 2];
		palette[i].r = (r_pct <= 100) ? (r * r_pct / 100) : (r + (63 - r) * (r_pct - 100) / 100);
		palette[i].g = (g_pct <= 100) ? (g * g_pct / 100) : (g + (63 - g) * (g_pct - 100) / 100);
		palette[i].b = (b_pct <= 100) ? (b * b_pct / 100) : (b + (63 - b) * (b_pct - 100) / 100);
	}
	set_palette(palette);
}

void pal_start_fade(int target_r, int target_g, int target_b, int steps)
{
	_fade_from[0] = _fade_cur[0];
	_fade_from[1] = _fade_cur[1];
	_fade_from[2] = _fade_cur[2];
	_fade_to[0] = target_r;
	_fade_to[1] = target_g;
	_fade_to[2] = target_b;
	_fade_step = 0;
	_fade_steps = steps;
	_fade_active = true;

	if (steps <= 0)
	{
		pal_set_fade(target_r, target_g, target_b);
		_fade_active = false;
	}
}

bool pal_update_fade()
{
	if (!_fade_active)
		return false;

	_fade_step++;
	if (_fade_step >= _fade_steps)
	{
		_fade_cur[0] = _fade_to[0];
		_fade_cur[1] = _fade_to[1];
		_fade_cur[2] = _fade_to[2];
		_fade_active = false;
	}
	else
	{
		_fade_cur[0] = _fade_from[0] + (_fade_to[0] - _fade_from[0]) * _fade_step / _fade_steps;
		_fade_cur[1] = _fade_from[1] + (_fade_to[1] - _fade_from[1]) * _fade_step / _fade_steps;
		_fade_cur[2] = _fade_from[2] + (_fade_to[2] - _fade_from[2]) * _fade_step / _fade_steps;
	}

	pal_set_fade(_fade_cur[0], _fade_cur[1], _fade_cur[2]);
	return _fade_active;
}

bool pal_fade_active()
{
	return _fade_active;
}
