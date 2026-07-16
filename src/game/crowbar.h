#pragma once

#include "actor.h"
#include "game_state.h"

struct crowbar_t : public actor_t
{
	game_state_t *game;
	int anim_tick = 0;

	crowbar_t(game_state_t *game) : game{game}
	{
		action_text = "Take crowbar";
		fpg_idx = 5;
		collidable = false;
	}

	void animate() override
	{
		if (!active)
			return;
		anim_tick++;
		if (anim_tick >= 3)
		{
			anim_tick = 0;
			fpg_idx = (fpg_idx == 5) ? 6 : 5;
		}
	}

	void on_action_pressed() override
	{
		if (!game->crowbar_picked)
			dialog_lines.push_back("With this I should be able to open the barn.");
		else
			dialog_lines.push_back("The crowbar is where I left it!");
		game->crowbar_picked = true;
		game->crowbar_alive = false;
		active = false;
	}
};
