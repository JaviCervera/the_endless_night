#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct barn_worker_t : public actionable_t
{
	game_state_t *game;

	barn_worker_t(texts_t *t, game_state_t *game, vec2_t pos) : actionable_t{BARN_WORKER_NAME, pos, BARN_WORKER_ID, t}, game{game}
	{
		halved = true;
		action_text = t->get("barn_worker_action");
	}

	void on_action_pressed() override
	{
		if (game->num_loop > 1)
			dialog_lines.push_back(t->get("barn_worker_loop_hint"));
		dialog_lines.push_back(t->get("barn_worker_msg"));
	}
};
