#pragma once

#include <ctime>
#include <vector>
#include "actionable.h"
#include "crowbar.h"
#include "entity_ids.h"
#include "game_state.h"
#include "../engine/tilemap.h"
#include "../engine/raycaster.h"

#define BARN_DOOR_OPEN_WINDOW 200

struct barn_door_t : public actionable_t
{
	game_state_t *game;
	tilemap_t *tilemap;
	raycaster_t *raycaster;
	uint32_t tile_x, tile_y;

	barn_door_t(game_state_t *game, tilemap_t *tilemap, raycaster_t *raycaster, uint32_t tile_x, uint32_t tile_y)
			: actionable_t{BARN_DOOR_NAME, vec2_t{real_t(tile_x + 0.5f), real_t(tile_y + 0.5f)}, 0},
				game{game},
				tilemap{tilemap},
				raycaster{raycaster},
				tile_x{tile_x},
				tile_y{tile_y}
	{
		collidable = false;
		action_text = "Open barn door";
		if (doors_opened)
			open_doors();
	}

	barn_door_t(const barn_door_t &) = delete;

	barn_door_t &operator=(const barn_door_t &) = delete;

	void on_action_pressed() override
	{
		if (!get_crowbar()->was_picked())
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
		open_doors();
		dialog_lines.push_back("The barn door creaks open.");
	}

private:
	inline static auto doors_opened = false;

	void open_doors()
	{
		const auto num = entity_t::num_entities_with_name(BARN_DOOR_NAME);
		for (size_t i = 0; i < num; ++i)
		{
			auto *d = static_cast<barn_door_t *>(entity_t::entity_with_name(BARN_DOOR_NAME, i));
			d->tilemap->tiles[d->tile_y * d->tilemap->map_size.x + d->tile_x] = 0;
			d->raycaster->tile({d->tile_x, d->tile_y}, 0);
			d->active = false;
		}
		doors_opened = true;
	}

	const crowbar_t *get_crowbar() const
	{
		return static_cast<crowbar_t *>(entity_t::entity_with_name(CROWBAR_NAME));
	}
};
