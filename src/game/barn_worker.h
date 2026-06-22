#pragma once

#include "actor.h"
#include "game_state.h"

struct barn_worker_t : public actor_t
{
	game_state_t *game;

	barn_worker_t(game_state_t *game) : game{game}
	{
		action_text = "Talk to barn worker";
		action_distance = real_t(1.5f);
	}

	void on_action_pressed() override
	{
		if (game->num_loop_in_state > 1)
			dialog_lines.push_back("I think there is something odd...");
		dialog_lines.push_back("A strange machine has landed down there.");
		dialog_lines.push_back("I work at the barn to the west, but I can't");
		dialog_lines.push_back("open the door. I have some parts there that");
		dialog_lines.push_back("might be useful to analyse that humming sound.");
	}
};
