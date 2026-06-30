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
		action_distance = real_t(1.5f);
		fpg_idx = 5;
		collidable = false;
	}

	void animate()
	{
		if (dead)
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
		if (game->player_last_picked_object != 1)
			dialog_lines.push_back("With this I should be able to open the barn.");
		else
			dialog_lines.push_back("The crowbar is where I left it!");
		game->player_last_picked_object = 1;
		game->crowbar_alive = false;
		dead = true;
	}
};
