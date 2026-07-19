#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct generator_t : public actionable_t
{
	game_state_t *game;
	int anim_tick = 0;

	generator_t(game_state_t *game, vec2_t pos) : 	actionable_t{GENERATOR_NAME, pos, GENERATOR_ID}, game{game}
	{
		collidable = false;
		action_text = "Take power generator";
		if (game->generator_picked)
			pos = game->player_end_pos;
		game->generator_alive = true;
	}

	void update() override
	{
		anim_tick++;
		if (anim_tick >= 3)
		{
			anim_tick = 0;
			fpg_id = (fpg_id == GENERATOR_ID) ? (GENERATOR_ID + 1) : GENERATOR_ID;
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
