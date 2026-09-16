#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "player.h"
#include "game_state.h"

struct generator_t : public actionable_t
{
	generator_t(player_t &player, vec2_t pos, game_state_t *game) : actionable_t{GENERATOR_NAME, pos, GENERATOR_ID}, player{&player}, game{game}
	{
		collidable = false;
		action_text = "Take power generator";
		if (already_picked)
		{
			this->pos = spawn_pos;
		}
	}

	~generator_t()
	{
		if (was_picked())
			spawn_pos = player->cam.pos;
	}

	void on_action_pressed() override
	{
		if (game->carried_generator != game_state_t::CARRIED_NONE)
		{
			dialog_lines.push_back("I am already carrying a generator");
			return;
		}

		dialog_lines.push_back("With four like this, I could restore the power.");
		dialog_lines.push_back("A signal is coming from the radio tower to the north...");
		pick();
	}

	bool was_picked() const
	{
		return !active;
	}

private:
	inline static auto already_picked = false;
	inline static auto spawn_pos = vec2_t{real_t(0.0f), real_t(0.0f)};

	player_t *player;
	game_state_t *game;

	void pick()
	{
		already_picked = true;
		active = false;
		game->carried_generator = game_state_t::CARRIED_BARN_GENERATOR;
	}
};
