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

	explicit compass_t(const texts_t *t)
	{
		// Cardinal letters at 0/12/24/36, ticks at the intercardinal points.
		m_pattern.assign(SECTORS, '-');
		m_pattern[0] = first_letter(t->get("compass_north"));
		m_pattern[12] = first_letter(t->get("compass_east"));
		m_pattern[24] = first_letter(t->get("compass_south"));
		m_pattern[36] = first_letter(t->get("compass_west"));
		m_pattern[6] = m_pattern[18] = m_pattern[30] = m_pattern[42] = '|';
	}

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
			const char glyph[2] = {m_pattern[static_cast<size_t>(idx)], '\0'};
			backbuffer.text(glyph, {static_cast<uint32_t>(x0 + i * CELL_W), y}, 15);
		}
	}

private:
	static char first_letter(const std::string &value)
	{
		return value.empty() ? '?' : value[0];
	}

	std::string m_pattern;

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
