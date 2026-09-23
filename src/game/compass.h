#pragma once

#include <cstdint>
#include <string>
#include "../engine/camera.h"
#include "../engine/pixmap.h"
#include "../engine/real.h"
#include "../engine/texts.h"
#include "../engine/vec2.h"
#include "../engine/viewport.h"

struct compass_t
{
	static constexpr int SECTORS = 48;
	static constexpr int CELL_W = 8;
	static constexpr int CELLS = 19;

	explicit compass_t(const texts_t *t) : t{t} {}

	void draw(pixmap_t &backbuffer, const camera_t &cam, viewport_t vp) const
	{
		const int sector = heading_sector(cam.dir);
		const int center = CELLS / 2;
		const int x0 = vp.x + (vp.w - CELLS * CELL_W) / 2;
		const uint32_t y = static_cast<uint32_t>(vp.y + vp.h + 6);

		for (int i = 0; i < CELLS; ++i)
		{
			int idx = (sector + i - center) % SECTORS;
			if (idx < 0)
				idx += SECTORS;
			const char glyph[2] = {pattern_char(idx), '\0'};
			backbuffer.text(glyph, {static_cast<uint32_t>(x0 + i * CELL_W), y}, 15);
		}
	}

private:
	char pattern_char(int idx) const
	{
		if (idx % 12 == 0)
			return first_letter(t->get(cardinal_key(idx)));
		return (idx % 6 == 0) ? '|' : '-';
	}

	static const char *cardinal_key(int idx)
	{
		switch (idx)
		{
		case 0:
			return "compass_north";
		case 12:
			return "compass_east";
		case 24:
			return "compass_south";
		default:
			return "compass_west";
		}
	}

	static char first_letter(const std::string &value)
	{
		return value.empty() ? '?' : value[0];
	}

	const texts_t *t;

	static int heading_sector(const vec2_t &dir)
	{
		static vec2_t sector_dirs[SECTORS];
		static bool initialized = false;
		if (!initialized)
		{
			for (int k = 0; k < SECTORS; ++k)
			{
				const real_t angle = real_t(2.0f * 3.14159265358979f * k / SECTORS);
				sector_dirs[k] = vec2_t{real_sin(angle), real_cos(angle)};
			}
			initialized = true;
		}

		int best = 0;
		real_t best_dot = dir.dot(sector_dirs[0]);
		for (int k = 1; k < SECTORS; ++k)
		{
			const real_t dot = dir.dot(sector_dirs[k]);
			if (dot > best_dot)
			{
				best_dot = dot;
				best = k;
			}
		}
		return best;
	}
};
