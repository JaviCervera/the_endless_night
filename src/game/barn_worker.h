#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct barn_worker_t : public actionable_t
{
	game_state_t *game;

	barn_worker_t(game_state_t *game, vec2_t pos) : actionable_t{BARN_WORKER_NAME, pos, BARN_WORKER_ID}, game{game}
	{
		halved = true;
		action_text = "Talk to barn worker";
	}

	void on_action_pressed() override
	{
		if (game->num_loop_in_state > 1)
			dialog_lines.push_back("I think there is something odd...");
		dialog_lines.push_back("A machine has landed down there. I work at the barn to the west, "
													 "but I can't open the door. I have some parts there that might be useful "
													 "to analyse that humming sound.");
	}
};
