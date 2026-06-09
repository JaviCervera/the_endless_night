#pragma once

#define RENDER_FLOOR 1
#define WALL_DARK_SHIFT 2 // Palette index shift for Y-axis wall darkening (0 = disabled, 1-15 = progressively darker)
#define FOG_ENABLED 1			// Set to 0 to compile out all fog code

#include <vector>
#include "fpg.h"
#include "pixmap.h"
#include "vec2.h"
#include "viewport.h"

struct camera_t;

// Sprite: a billboard object in the world. fpg_idx is the 0-based FPG map index
// for its texture. Palette color 0 is treated as transparent.
struct sprite_t
{
	vec2_t pos;			// World position
	uint8_t fpg_idx; // FPG map index (0-based) for the sprite texture
};

struct raycaster_t
{
	raycaster_t(uvec2_t map_size, const fpg_t &fpg);

	void tile(uvec2_t pos, uint8_t wall_type);
	void floor(uvec2_t pos, uint8_t floor_type);
	uint8_t tile(uvec2_t pos) const;
	uint8_t floor(uvec2_t pos) const;

	// Renders into the given viewport sub-region. Pass viewport_t{} for full backbuffer.
	// detail controls interlaced rendering: 2 = full (default), 0 = even columns/rows only,
	// 1 = odd columns/rows only. Combining alternating frames gives a full image at half cost.
	void render(const camera_t &cam, pixmap_t &backbuffer, viewport_t vp = {}, int detail = 2);

	const fpg_t *fpg;
	uvec2_t map_size;
	// Flattened: world_map[x * map_size.y + y] / floor_map[x * map_size.y + y].
	// Single heap block, sequential DDA access, fewer indirections than vector<vector>.
	std::vector<uint8_t> world_map;
	std::vector<uint8_t> floor_map;
	std::vector<sprite_t> sprites;
	palcolor_t ceiling_color = 0;

#if FOG_ENABLED
	palcolor_t fog_color = 0;
	real_t fog_start = real_t(8);
	real_t fog_end = real_t(16);
#endif

private:
	// Reused across frames to avoid per-frame heap allocation.
	std::vector<real_t> z_buf;
	std::vector<size_t> sp_order;

	// Bounds-checked-index-free accessors for use in hot inner loops (DDA, floor
	// pixel loop) where the caller has already verified (x, y) is in range.
	uint8_t tile_unsafe(int x, int y) const;
	uint8_t floor_unsafe(int x, int y) const;
};

inline void raycaster_t::tile(uvec2_t pos, uint8_t wall_type)
{
	if (pos.x < map_size.x && pos.y < map_size.y)
		world_map[static_cast<size_t>(pos.x) * map_size.y + pos.y] = wall_type;
}

inline void raycaster_t::floor(uvec2_t pos, uint8_t floor_type)
{
	if (pos.x < map_size.x && pos.y < map_size.y)
		floor_map[static_cast<size_t>(pos.x) * map_size.y + pos.y] = floor_type;
}

inline uint8_t raycaster_t::tile(uvec2_t pos) const
{
	if (pos.x < map_size.x && pos.y < map_size.y)
		return world_map[static_cast<size_t>(pos.x) * map_size.y + pos.y];
	return 0;
}

inline uint8_t raycaster_t::floor(uvec2_t pos) const
{
	if (pos.x < map_size.x && pos.y < map_size.y)
		return floor_map[static_cast<size_t>(pos.x) * map_size.y + pos.y];
	return 0;
}

inline uint8_t raycaster_t::tile_unsafe(int x, int y) const
{
	return world_map[static_cast<size_t>(x) * map_size.y + y];
}

inline uint8_t raycaster_t::floor_unsafe(int x, int y) const
{
	return floor_map[static_cast<size_t>(x) * map_size.y + y];
}
