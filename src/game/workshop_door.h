#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct workshop_door_t : public actionable_t
{
	workshop_door_t(game_state_t *game, vec2_t pos)
			: actionable_t{WORKSHOP_DOOR_NAME, pos, 0}, game{game}
	{
		action_text = "Enter workshop code";
		collidable = false;
	}

	void on_action_pressed() override
	{
		if (!game->tree_note_read)
		{
			dialog_lines.push_back("I don't know the code");
			return;
		}

		game->saved_elapsed_ticks = game->elapsed_ticks();
		game->phase = game_state_t::PHASE_WORKSHOP_MINIGAME;
	}

private:
	game_state_t *game;
};