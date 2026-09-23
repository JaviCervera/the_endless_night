#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct tree_note_t : public actionable_t
{
	tree_note_t(texts_t *t, game_state_t *game, vec2_t pos)
			: actionable_t{TREE_NOTE_NAME, pos, TREE_NOTE_ID, t}, game{game}
	{
		halved = true;
		collidable = false;
		action_text = t->get("tree_note_action");
	}

	void on_action_pressed() override
	{
		if (game->tree_note_read)
		{
			dialog_lines.push_back(t->get("tree_note_reread"));
			return;
		}

		game->tree_note_read = true;
		dialog_lines.push_back(t->get("tree_note_read"));
	}

private:
	game_state_t *game;
};
