#pragma once

#include <ctime>
#include <vector>
#include "actor.h"
#include "game_state.h"
#include "../engine/tilemap.h"
#include "../engine/raycaster.h"

struct barn_door_t : public actor_t
{
	game_state_t *game;
	tilemap_t *tilemap;
	raycaster_t *raycaster;
	uint32_t tile_x, tile_y;

	inline static std::vector<barn_door_t*> all;

	barn_door_t(game_state_t *game, tilemap_t *tilemap, raycaster_t *raycaster)
			: game{game}, tilemap{tilemap}, raycaster{raycaster}
	{
		action_text = "Open barn door";
		collidable = false;
		all.push_back(this);
	}

	barn_door_t(const barn_door_t &) = delete;

	~barn_door_t() override
	{
		for (auto it = all.begin(); it != all.end(); ++it)
			if (*it == this)
			{
				all.erase(it);
				break;
			}
	}

	barn_door_t &operator=(const barn_door_t &) = delete;

	void on_action_pressed() override
	{
		if (!game->crowbar_picked)
		{
			dialog_lines.push_back("I need something to pry this open.");
			return;
		}
		int elapsed = int((clock() - game->loop_start_clock) * 20 / CLOCKS_PER_SEC);
		if (elapsed >= BARN_DOOR_OPEN_WINDOW)
		{
			dialog_lines.push_back("Something is blocking the door. It was not there before.");
			return;
		}
		game->barn_doors_open = true;
		game->advance_state();
		for (auto *d : all)
		{
			d->tilemap->tiles[d->tile_y * d->tilemap->map_size.x + d->tile_x] = 0;
			d->raycaster->tile({d->tile_x, d->tile_y}, 0);
			d->active = false;
		}
		dialog_lines.push_back("The barn door creaks open.");
	}
};
