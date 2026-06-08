#pragma once

#include "../engine/camera.h"
#include "../engine/screen.h"
#include "../engine/tilemap.h"

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

	void update()
	{
		auto wall_at = [this](real_t px, real_t py) -> bool
		{
			const int x = int(px);
			const int y = int(py);
			if (x < 0 || x >= static_cast<int>(tilemap->map_size.x) || y < 0 || y >= static_cast<int>(tilemap->map_size.y))
				return true;
			return tilemap->tile_at(static_cast<uint32_t>(x), static_cast<uint32_t>(y)) != 0;
		};

		if (screen_key(SCREEN_KEY_UP))
		{
			const real_t new_x = cam.pos.x + cam.dir.x * move_speed;
			const real_t new_y = cam.pos.y + cam.dir.y * move_speed;
			if (!wall_at(new_x + radius, cam.pos.y + radius) && !wall_at(new_x + radius, cam.pos.y - radius) &&
					!wall_at(new_x - radius, cam.pos.y + radius) && !wall_at(new_x - radius, cam.pos.y - radius))
				cam.pos.x = new_x;
			if (!wall_at(cam.pos.x + radius, new_y + radius) && !wall_at(cam.pos.x + radius, new_y - radius) &&
					!wall_at(cam.pos.x - radius, new_y + radius) && !wall_at(cam.pos.x - radius, new_y - radius))
				cam.pos.y = new_y;
		}
		if (screen_key(SCREEN_KEY_DOWN))
		{
			const real_t new_x = cam.pos.x - cam.dir.x * move_speed;
			const real_t new_y = cam.pos.y - cam.dir.y * move_speed;
			if (!wall_at(new_x + radius, cam.pos.y + radius) && !wall_at(new_x + radius, cam.pos.y - radius) &&
					!wall_at(new_x - radius, cam.pos.y + radius) && !wall_at(new_x - radius, cam.pos.y - radius))
				cam.pos.x = new_x;
			if (!wall_at(cam.pos.x + radius, new_y + radius) && !wall_at(cam.pos.x + radius, new_y - radius) &&
					!wall_at(cam.pos.x - radius, new_y + radius) && !wall_at(cam.pos.x - radius, new_y - radius))
				cam.pos.y = new_y;
		}
		if (screen_key(SCREEN_KEY_RIGHT))
		{
			const real_t old_dir_x = cam.dir.x;
			const real_t old_plane_x = cam.plane.x;
			cam.dir.x = cam.dir.x * real_cos(-rot_speed) - cam.dir.y * real_sin(-rot_speed);
			cam.dir.y = old_dir_x * real_sin(-rot_speed) + cam.dir.y * real_cos(-rot_speed);
			cam.plane.x = cam.plane.x * real_cos(-rot_speed) - cam.plane.y * real_sin(-rot_speed);
			cam.plane.y = old_plane_x * real_sin(-rot_speed) + cam.plane.y * real_cos(-rot_speed);
		}
		if (screen_key(SCREEN_KEY_LEFT))
		{
			const real_t old_dir_x = cam.dir.x;
			const real_t old_plane_x = cam.plane.x;
			cam.dir.x = cam.dir.x * real_cos(rot_speed) - cam.dir.y * real_sin(rot_speed);
			cam.dir.y = old_dir_x * real_sin(rot_speed) + cam.dir.y * real_cos(rot_speed);
			cam.plane.x = cam.plane.x * real_cos(rot_speed) - cam.plane.y * real_sin(rot_speed);
			cam.plane.y = old_plane_x * real_sin(rot_speed) + cam.plane.y * real_cos(rot_speed);
		}
	}
};
