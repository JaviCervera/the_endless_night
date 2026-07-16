#pragma once

#include "actor.h"
#include "game_state.h"

struct generator_t : public actor_t
{
	game_state_t *game;
	int anim_tick = 0;

	generator_t(game_state_t *game) : game{game}
	{
		action_text = "Take power generator";
		fpg_idx = 12;
		collidable = false;
	}

	void update() override
	{
		anim_tick++;
		if (anim_tick >= 3)
		{
			anim_tick = 0;
			fpg_idx = (fpg_idx == 12) ? 13 : 12;
		}
	}

	void on_action_pressed() override
	{
		dialog_lines.push_back("With four like this, I can restore the power.");
		dialog_lines.push_back("A signal is coming from the radio tower to the north...");
		game->generator_picked = true;
		game->generator_alive = false;
		active = false;
	}
};
