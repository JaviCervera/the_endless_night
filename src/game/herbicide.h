#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct herbicide_t : public actionable_t
{
	herbicide_t(texts_t *t, game_state_t *game, vec2_t pos) : actionable_t{HERBICIDE_NAME, pos, HERBICIDE_ID, t}, game{game}
	{
		halved = true;
		collidable = false;
		action_text = t->get("herbicide_action");
		if (game->vines_cleared || game->num_loop % 2 != 0)
			active = false;
	}

	void on_action_pressed() override
	{
		game->herbicide_held = true;
		active = false;
		dialog_lines.push_back(t->get("herbicide_taken"));
	}

private:
	game_state_t *game;
};
