#pragma once

#include <array>
#include <fstream>
#include <vector>
#include "pixmap.h"

struct fpg_t
{
	static fpg_t load(const char *filename, bool column_cache);
	size_t num_maps() const;
	const pixmap_t *map(size_t index) const;

private:
	std::vector<pixmap_t> m_maps;
};

inline fpg_t fpg_t::load(const char *filename, bool column_cache)
{
	auto fpg = fpg_t{};
	auto input = std::ifstream{filename, std::ios::binary};
	if (input)
	{
		auto id = std::array<char, 3>{};
		auto hex = int32_t{};
		auto ver = uint8_t{};
		input.read(id.data(), sizeof(id));
		input.read(reinterpret_cast<char *>(&hex), sizeof(hex));
		input.read(reinterpret_cast<char *>(&ver), sizeof(ver));

		auto pal = std::array<uint8_t, 256 * 3 + 16 * 36>{};
		input.read(reinterpret_cast<char *>(pal.data()), pal.size());

		while (!input.eof())
		{
			auto code = int32_t{};
			auto length = int32_t{};
			auto desc = std::array<char, 32>{};
			auto name = std::array<char, 12>{};
			auto width = int32_t{};
			auto height = int32_t{};
			auto num_points = int32_t{};
			input.read(reinterpret_cast<char *>(&code), sizeof(code));
			input.read(reinterpret_cast<char *>(&length), sizeof(length));
			input.read(reinterpret_cast<char *>(desc.data()), desc.size());
			input.read(reinterpret_cast<char *>(name.data()), name.size());
			input.read(reinterpret_cast<char *>(&width), sizeof(width));
			input.read(reinterpret_cast<char *>(&height), sizeof(height));
			input.read(reinterpret_cast<char *>(&num_points), sizeof(num_points));
			for (auto i = 0; i < num_points; ++i)
			{
				auto coords = std::array<uint16_t, 2>{};
				input.read(reinterpret_cast<char *>(coords.data()), coords.size());
			}
			auto pixmap = pixmap_t{uvec2_t(width, height), 0};
			for (auto y = 0; y < height; ++y)
			{
				for (auto x = 0; x < width; ++x)
				{
					auto color = uint8_t{};
					input.read(reinterpret_cast<char *>(&color), sizeof(color));
					pixmap.pixel(uvec2_t(x, y), color);
				}
			}
			if (column_cache)
				pixmap.build_col_cache();
			fpg.m_maps.push_back(std::move(pixmap));
		}
	}
	return fpg;
}

inline size_t fpg_t::num_maps() const
{
	return m_maps.size();
}

inline const pixmap_t *fpg_t::map(size_t index) const
{
	if (index >= num_maps())
	{
		return nullptr;
	}
	return &m_maps[index];
}
