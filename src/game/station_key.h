#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct station_key_t : public actionable_t
{
	station_key_t(texts_t *t, game_state_t *game, vec2_t pos) : actionable_t{KEY_NAME, pos, KEY_ID, t}, game{game}
	{
		halved = true;
		collidable = false;
		action_text = t->get("station_key_action");
		if (game->station_door_opened)
			active = false;
	}

	void on_action_pressed() override
	{
		game->station_key_held = true;
		active = false;
		dialog_lines.push_back(t->get("station_key_taken"));
	}

private:
	game_state_t *game;
};
