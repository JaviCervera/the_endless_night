#pragma once

#include "../engine/camera.h"
#include "../engine/tilemap.h"
#include "../engine/sprite.h"
#include "input.h"
#include "game_state.h"

struct player_t
{
	const tilemap_t *tilemap;
	camera_t cam;
	real_t move_speed;
	real_t rot_speed;
	real_t radius;

	player_t(const tilemap_t &tilemap, real_t move_speed, real_t rot_speed, real_t radius)
			: tilemap{&tilemap},
				cam{
						vec2_t{real_t(0.0f), real_t(0.0f)},
						vec2_t{real_t(0.0f), real_t(1.0f)},
						vec2_t{real_t(0.66f), real_t(0.0f)}},
				move_speed{move_speed},
				rot_speed{rot_speed},
				radius{radius} {}

	void position(vec2_t pos)
	{
		cam.pos = pos;
	}

	void update(const input_t &input, const game_state_t &game)
	{
		if (game.player_blocked)
			return;

		auto wall_at = [this](real_t px, real_t py) -> bool
		{
			const auto x = static_cast<int>(px);
			const auto y = static_cast<int>(py);
			if (x < 0 || x >= static_cast<int>(tilemap->map_size.x) || y < 0 || y >= static_cast<int>(tilemap->map_size.y))
				return true;
			return tilemap->tile_at(static_cast<uint32_t>(x), static_cast<uint32_t>(y)) != 0;
		};

		auto entity_at = [this](real_t px, real_t py) -> bool
		{
			for (const auto *s : sprite_t::all)
			{
				if (!s->collidable)
					continue;
				const auto dx = px - s->pos.x;
				const auto dy = py - s->pos.y;
				if (dx*dx + dy*dy < real_t(0.25f))
					return true;
			}
			return false;
		};

		if (input.forward != 0)
		{
			const auto new_x = cam.pos.x + cam.dir.x * move_speed * input.forward;
			const auto new_y = cam.pos.y + cam.dir.y * move_speed * input.forward;
			if (!wall_at(new_x + radius, cam.pos.y + radius) && !wall_at(new_x + radius, cam.pos.y - radius) &&
					!wall_at(new_x - radius, cam.pos.y + radius) && !wall_at(new_x - radius, cam.pos.y - radius) &&
					!entity_at(new_x, cam.pos.y))
				cam.pos.x = new_x;
			if (!wall_at(cam.pos.x + radius, new_y + radius) && !wall_at(cam.pos.x + radius, new_y - radius) &&
					!wall_at(cam.pos.x - radius, new_y + radius) && !wall_at(cam.pos.x - radius, new_y - radius) &&
					!entity_at(cam.pos.x, new_y))
				cam.pos.y = new_y;
		}
		if (input.turn != 0)
		{
			const auto old_dir_x = cam.dir.x;
			const auto old_plane_x = cam.plane.x;
			const auto c = real_cos(rot_speed * -input.turn);
			const auto s = real_sin(rot_speed * -input.turn);
			cam.dir.x = cam.dir.x * c - cam.dir.y * s;
			cam.dir.y = old_dir_x * s + cam.dir.y * c;
			cam.plane.x = cam.plane.x * c - cam.plane.y * s;
			cam.plane.y = old_plane_x * s + cam.plane.y * c;
		}
	}
};
