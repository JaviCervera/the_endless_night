#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct farmer_t : public actionable_t
{
	game_state_t *game;

	farmer_t(texts_t *t, game_state_t *game, vec2_t pos) : actionable_t{FARMER_NAME, pos, FARMER_ID, t}, game{game}
	{
		halved = true;
		action_text = t->get("farmer_action");
	}

	void on_action_pressed() override
	{
		if (game->num_loop > 1)
			dialog_lines.push_back(t->get("farmer_loop_hint"));
		dialog_lines.push_back(t->get("farmer_msg"));
	}
};
