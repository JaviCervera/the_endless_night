#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "player.h"
#include "game_state.h"

struct generator_t : public actionable_t
{
	generator_t(texts_t *t, player_t &player, vec2_t pos, game_state_t *game) : actionable_t{GENERATOR_NAME, pos, GENERATOR_ID, t}, player{&player}, game{game}
	{
		collidable = false;
		action_text = t->get("generator_action");
		if (game->barn_generator_picked)
		{
			this->pos = game->barn_generator_spawn_pos;
		}
	}

	~generator_t()
	{
		if (was_picked())
			game->barn_generator_spawn_pos = player->cam.pos;
	}

	void on_action_pressed() override
	{
		if (game->carried_generator != game_state_t::CARRIED_NONE)
		{
			dialog_lines.push_back(t->get("generator_already_carrying"));
			return;
		}

		dialog_lines.push_back(t->get("generator_taken"));
		dialog_lines.push_back(t->get("generator_signal"));
		pick();
	}

	bool was_picked() const
	{
		return !active;
	}

private:
	player_t *player;
	game_state_t *game;

	void pick()
	{
		game->barn_generator_picked = true;
		active = false;
		game->carried_generator = game_state_t::CARRIED_BARN_GENERATOR;
	}
};
