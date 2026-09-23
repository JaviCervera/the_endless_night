#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct vine_t : public actionable_t
{
	vine_t(texts_t *t, game_state_t *game, vec2_t pos) : actionable_t{VINE_NAME, pos, VINE_ID, t}, game{game}
	{
		action_text = t->get("vine_action");
		if (game->vines_cleared)
		{
			active = false;
			collidable = false;
		}
	}

	void update() override
	{
		action_text = game->herbicide_held ? t->get("vine_action_herbicide") : t->get("vine_action");
	}

	void on_action_pressed() override
	{
		if (game->herbicide_held)
		{
			game->vines_cleared = true;
			dialog_lines.push_back(t->get("vine_herbicide_used"));
			return;
		}
		dialog_lines.push_back(t->get("vine_blocked"));
	}

private:
	game_state_t *game;
};
