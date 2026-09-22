#pragma once

#include <vector>
#include <allegro.h>
#include "pal.h"
#include "vec2.h"

struct pixmap_t
{
	pixmap_t(uvec2_t size, palcolor_t color);
	pixmap_t(const pixmap_t &other) = delete;
	pixmap_t(pixmap_t &&other);
	~pixmap_t();
	pixmap_t &operator=(pixmap_t &&other);
	pixmap_t &operator=(const pixmap_t &other) = delete;
	uvec2_t size() const;
	palcolor_t pixel(uvec2_t pos) const;
	void pixel(uvec2_t pos, palcolor_t color);
	void line(uvec2_t origin, uvec2_t end, palcolor_t color);
	void rect(uvec2_t origin, uvec2_t size, palcolor_t color);
	void rectfill(uvec2_t origin, uvec2_t size, palcolor_t color);
	void oval(uvec2_t origin, uvec2_t size, palcolor_t color);
	void ovalfill(uvec2_t origin, uvec2_t size, palcolor_t color);
	void fill(palcolor_t color);
	void blit(const pixmap_t &other, ivec2_t origin);
	void tile(const pixmap_t &source, ivec2_t origin, uvec2_t rect_pos, uvec2_t rect_size);
	void text(const char *str, uvec2_t pos, palcolor_t color);

	void build_col_cache();
	const palcolor_t *column(int x) const;

	// Direct row/stride access. Use these in hot inner loops to avoid the
	// per-pixel y*stride multiply that pixel(uvec2_t,..) would do.
	palcolor_t *row(int y);
	const palcolor_t *row(int y) const;
	int stride() const;

private:
	BITMAP *m_bmp;
	int32_t m_col_h; // height saved here after m_bmp is freed by build_col_cache()
	std::vector<palcolor_t> m_col_pixels;
};

inline uvec2_t pixmap_t::size() const
{
	if (m_bmp)
		return uvec2_t(m_bmp->w, m_bmp->h);
	return uvec2_t(static_cast<uint32_t>(m_col_pixels.size() / m_col_h), static_cast<uint32_t>(m_col_h));
}
inline palcolor_t pixmap_t::pixel(uvec2_t pos) const
{
	if (m_bmp)
		return palcolor_t(m_bmp->line[pos.y][pos.x]);
	return m_col_pixels[static_cast<size_t>(pos.x * m_col_h + pos.y)];
}
inline void pixmap_t::pixel(uvec2_t pos, palcolor_t color)
{
	if (m_bmp)
	{
		m_bmp->line[pos.y][pos.x] = color;
		return;
	}
	m_col_pixels[static_cast<size_t>(pos.x * m_col_h + pos.y)] = color;
}

inline const palcolor_t *pixmap_t::column(int x) const
{
	// Use m_col_h directly: size() recomputes m_col_pixels.size() / m_col_h,
	// which lowered to a hardware div on every call in the floor loop.
	return m_col_pixels.data() + static_cast<size_t>(x) * static_cast<size_t>(m_col_h);
}

inline palcolor_t *pixmap_t::row(int y)
{
	return m_bmp->line[y];
}

inline const palcolor_t *pixmap_t::row(int y) const
{
	return m_bmp->line[y];
}

inline int pixmap_t::stride() const
{
	return m_bmp->w;
}
