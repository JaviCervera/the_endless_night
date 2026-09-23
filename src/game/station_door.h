#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct station_door_t : public actionable_t
{
	station_door_t(texts_t *t, game_state_t *game, vec2_t pos)
			: actionable_t{STATION_DOOR_NAME, pos, STATION_DOOR_ID, t}, game{game}
	{
		action_text = t->get("station_door_action");
		if (game->station_door_opened)
		{
			active = false;
			collidable = false;
		}
	}

	void on_action_pressed() override
	{
		if (!game->station_key_held)
		{
			dialog_lines.push_back(t->get("station_door_locked"));
			return;
		}

		game->station_door_opened = true;
		open_doors();
		dialog_lines.push_back(t->get("station_door_opened"));
	}

private:
	game_state_t *game;

	void open_doors()
	{
		const auto num = entity_t::num_entities_with_name(STATION_DOOR_NAME);
		for (size_t i = 0; i < num; ++i)
		{
			auto *door = static_cast<station_door_t *>(entity_t::entity_with_name(STATION_DOOR_NAME, i));
			door->active = false;
			door->collidable = false;
		}
	}
};
