#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct radio_station_t : public actionable_t
{
	radio_station_t(vec2_t pos, game_state_t *game) : actionable_t{RADIO_STATION_NAME, pos, RADIO_STATION_ID}, game{game}
	{
		collidable = true;
		action_text = "Take power generator";
	}

	void on_action_pressed() override
	{
		if (game->carried_generator != game_state_t::CARRIED_NONE)
		{
			dialog_lines.push_back("I am already carrying a generator");
			return;
		}

		if (game->generator_placed[1])
		{
			dialog_lines.push_back("I already have the power generator");
			return;
		}

		if (!already_picked)
		{
			dialog_lines.push_back("I got a power generator!");
			already_picked = true;
			game->carried_generator = game_state_t::CARRIED_RADIO;
		}
		else
		{
			dialog_lines.push_back("I already have the power generator");
		}
	}

	static void reset_picked()
	{
		already_picked = false;
	}

private:
	inline static auto already_picked = false;
	game_state_t *game;
};
