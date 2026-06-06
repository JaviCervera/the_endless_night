#include "pixmap.h"

pixmap_t::pixmap_t(uvec2_t size, palcolor_t color)
		: m_bmp{create_bitmap(size.x, size.y)},
			m_col_h{0}
{
	fill(color);
}

pixmap_t::pixmap_t(pixmap_t &&other)
		: m_bmp(other.m_bmp),
			m_col_h(other.m_col_h),
			m_col_pixels(std::move(other.m_col_pixels))
{
	other.m_bmp = nullptr;
	other.m_col_h = 0;
}

pixmap_t::~pixmap_t()
{
	if (m_bmp)
	{
		destroy_bitmap(m_bmp);
	}
}

pixmap_t &pixmap_t::operator=(pixmap_t &&other)
{
	m_bmp = other.m_bmp;
	m_col_h = other.m_col_h;
	m_col_pixels = std::move(other.m_col_pixels);
	other.m_bmp = nullptr;
	other.m_col_h = 0;
	return *this;
}

void pixmap_t::fill(palcolor_t color)
{
	clear_to_color(m_bmp, color);
}

void pixmap_t::line(uvec2_t origin, uvec2_t end, palcolor_t color)
{
	::line(m_bmp, origin.x, origin.y, end.x, end.y, color);
}

void pixmap_t::rect(uvec2_t origin, uvec2_t size, palcolor_t color)
{
	::rect(m_bmp, origin.x, origin.y, origin.x + size.x - 1, origin.y + size.y - 1, color);
}

void pixmap_t::rectfill(uvec2_t origin, uvec2_t size, palcolor_t color)
{
	::rectfill(m_bmp, origin.x, origin.y, origin.x + size.x - 1, origin.y + size.y - 1, color);
}

void pixmap_t::oval(uvec2_t origin, uvec2_t size, palcolor_t color)
{
	ellipse(m_bmp, origin.x + size.x / 2, origin.y + size.y / 2, size.x / 2, size.y / 2, color);
}

void pixmap_t::ovalfill(uvec2_t origin, uvec2_t size, palcolor_t color)
{
	ellipsefill(m_bmp, origin.x + size.x / 2, origin.y + size.y / 2, size.x / 2, size.y / 2, color);
}

void pixmap_t::blit(const pixmap_t &other, ivec2_t origin)
{
	draw_sprite(m_bmp, other.m_bmp, origin.x, origin.y);
}

void pixmap_t::tile(const pixmap_t &source, ivec2_t origin, uvec2_t rect_pos, uvec2_t rect_size)
{
	const int sw = source.m_bmp->w;
	const int sh = source.m_bmp->h;
	const int rx0 = rect_pos.x, ry0 = rect_pos.y;
	const int rx1 = rx0 + rect_size.x - 1, ry1 = ry0 + rect_size.y - 1;
	const int start_x = rx0 - ((origin.x % sw + sw) % sw);
	const int start_y = ry0 - ((origin.y % sh + sh) % sh);
	set_clip_rect(m_bmp, rx0, ry0, rx1, ry1);
	for (int ty = start_y; ty <= ry1; ty += sh)
		for (int tx = start_x; tx <= rx1; tx += sw)
			::blit(source.m_bmp, m_bmp, 0, 0, tx, ty, sw, sh);
	set_clip_rect(m_bmp, 0, 0, m_bmp->w - 1, m_bmp->h - 1);
}

void pixmap_t::text(const char *str, uvec2_t pos, palcolor_t color)
{
	textout_ex(m_bmp, font, str, pos.x, pos.y, color, -1);
}

void pixmap_t::build_col_cache()
{
	const int w = m_bmp->w;
	const int h = m_bmp->h;
	m_col_h = h;
	m_col_pixels.resize(static_cast<size_t>(w * h));
	for (int x = 0; x < w; ++x)
		for (int y = 0; y < h; ++y)
			m_col_pixels[static_cast<size_t>(x * h + y)] = m_bmp->line[y][x];
	// Free the row-major bitmap — col_pixels is the only copy now.
	destroy_bitmap(m_bmp);
	m_bmp = nullptr;
}
