#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct vine_t : public actionable_t
{
	vine_t(game_state_t *game, vec2_t pos) : actionable_t{VINE_NAME, pos, VINE_ID}, game{game}
	{
		action_text = "Access field";
		if (game->vines_cleared)
		{
			active = false;
			collidable = false;
		}
	}

	void update() override
	{
		action_text = game->herbicide_held ? "Kill vines with herbicide" : "Access field";
	}

	void on_action_pressed() override
	{
		if (game->herbicide_held)
		{
			game->vines_cleared = true;
			dialog_lines.push_back("Nothing seems to happen.");
			return;
		}
		dialog_lines.push_back("The vines block the way");
	}

private:
	game_state_t *game;
};
