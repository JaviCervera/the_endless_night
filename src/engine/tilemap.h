#pragma once

#include <cstdint>
#include <fstream>
#include <vector>
#include "vec2.h"

struct tilemap_t
{
	uvec2_t map_size;
	std::vector<uint8_t> floor;
	std::vector<uint8_t> tiles;
	std::vector<uint8_t> entities;

	uint8_t floor_at(uint32_t x, uint32_t y) const
	{
		if (x >= map_size.x || y >= map_size.y)
			return 0;
		return floor[y * map_size.x + x];
	}

	uint8_t tile_at(uint32_t x, uint32_t y) const
	{
		if (x >= map_size.x || y >= map_size.y)
			return 0;
		return tiles[y * map_size.x + x];
	}

	uint8_t entity_at(uint32_t x, uint32_t y) const
	{
		if (x >= map_size.x || y >= map_size.y)
			return 0;
		return entities[y * map_size.x + x];
	}
};

inline tilemap_t load_tilemap(const char *filename)
{
	tilemap_t map{};
	std::ifstream f(filename, std::ios::binary);
	if (!f)
		return map;

	f.read(reinterpret_cast<char *>(&map.map_size.x), sizeof(uint32_t));
	f.read(reinterpret_cast<char *>(&map.map_size.y), sizeof(uint32_t));

	const size_t count = static_cast<size_t>(map.map_size.x) * map.map_size.y;
	map.floor.resize(count);
	map.tiles.resize(count);
	map.entities.resize(count);

	f.read(reinterpret_cast<char *>(map.floor.data()), count);
	f.read(reinterpret_cast<char *>(map.tiles.data()), count);
	f.read(reinterpret_cast<char *>(map.entities.data()), count);

	return map;
}
