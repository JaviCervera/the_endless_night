#pragma once

#include "actor.h"
#include "game_state.h"

struct farmer_t : public actor_t
{
	game_state_t *game;

	farmer_t(game_state_t *game) : game{game}
	{
		action_text = "Talk to farmer";
	}

	void on_action_pressed() override
	{
		if (game->num_loop_in_state > 1)
			dialog_lines.push_back("I have the feeling that I have done this before...");
		dialog_lines.push_back("Something strange has happened in the fields to the south, but the path is closed.");
	}
};
