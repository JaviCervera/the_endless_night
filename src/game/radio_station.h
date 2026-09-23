#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct radio_station_t : public actionable_t
{
	radio_station_t(texts_t *t, vec2_t pos, game_state_t *game) : actionable_t{RADIO_STATION_NAME, pos, RADIO_STATION_ID, t}, game{game}
	{
		collidable = true;
		action_text = t->get("radio_station_action");
	}

	void on_action_pressed() override
	{
		if (game->carried_generator != game_state_t::CARRIED_NONE)
		{
			dialog_lines.push_back(t->get("radio_station_already_carrying"));
			return;
		}

		if (game->generator_placed[game_state_t::CARRIED_RADIO_GENERATOR])
		{
			dialog_lines.push_back(t->get("radio_station_already_have"));
			return;
		}

		dialog_lines.push_back(t->get("radio_station_taken"));
		game->carried_generator = game_state_t::CARRIED_RADIO_GENERATOR;
	}

private:
	game_state_t *game;
};
