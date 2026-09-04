#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct tree_note_t : public actionable_t
{
	tree_note_t(game_state_t *game, vec2_t pos)
			: actionable_t{TREE_NOTE_NAME, pos, TREE_NOTE_ID}, game{game}
	{
		halved = true;
		collidable = false;
		action_text = "Read note";
	}

	void on_action_pressed() override
	{
		if (game->tree_note_read)
		{
			dialog_lines.push_back("I have good memory; I remember the code for the workshop is 1982");
			return;
		}

		game->tree_note_read = true;
		dialog_lines.push_back("\"I keep forgetting the access code to the workshop. I better write it down: 1982 - Ted the mechanic\"");
	}

private:
	game_state_t *game;
};
