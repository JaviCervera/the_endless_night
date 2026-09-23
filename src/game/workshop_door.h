#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct workshop_door_t : public actionable_t
{
	workshop_door_t(texts_t *t, game_state_t *game, vec2_t pos)
			: actionable_t{WORKSHOP_DOOR_NAME, pos, 0, t}, game{game}
	{
		action_text = t->get("workshop_door_action");
		collidable = false;
	}

	void on_action_pressed() override
	{
		if (!game->tree_note_read)
		{
			dialog_lines.push_back(t->get("workshop_door_unknown_code"));
			return;
		}

		if (game->carried_generator != game_state_t::CARRIED_NONE)
		{
			dialog_lines.push_back(t->get("workshop_door_already_carrying"));
			return;
		}

		if (game->generator_placed[game_state_t::CARRIED_WORKSHOP_GENERATOR])
		{
			dialog_lines.push_back(t->get("workshop_door_already_have"));
			return;
		}

		game->saved_elapsed_ticks = game->elapsed_ticks();
		game->request_workshop_minigame = true;
	}

private:
	game_state_t *game;
};