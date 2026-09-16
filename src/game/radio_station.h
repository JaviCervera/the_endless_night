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

		if (game->generator_placed[game_state_t::CARRIED_RADIO_GENERATOR])
		{
			dialog_lines.push_back("I already have the power generator");
			return;
		}

		dialog_lines.push_back("I got a power generator!");
		game->carried_generator = game_state_t::CARRIED_RADIO_GENERATOR;
	}

private:
	game_state_t *game;
};
