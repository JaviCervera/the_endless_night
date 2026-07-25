#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "player.h"

struct generator_t : public actionable_t
{
	generator_t(player_t &player, vec2_t pos) : actionable_t{GENERATOR_NAME, pos, GENERATOR_ID}, player{&player}
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

	void pick()
	{
		already_picked = true;
		active = false;
	}
};
