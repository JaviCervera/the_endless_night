#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct power_tower_t : public actionable_t
{
	power_tower_t(vec2_t pos, int index, game_state_t *game) : actionable_t{POWER_TOWER_NAME, pos, POWER_TOWER_ID}, tower_index(index), game(game)
	{
		collidable = true;
		action_text = "Place power generator";
	}

	void on_action_pressed() override
	{
		if (game->tower_generator[tower_index] != -1)
		{
			dialog_lines.push_back("This tower already has a generator");
			return;
		}

		if (game->carried_generator == game_state_t::CARRIED_NONE)
		{
			if (game->num_generators_placed == 3)
			{
				game->saved_elapsed_ticks = game->elapsed_ticks();
				game->request_tower_minigame = true;
				return;
			}
			else
			{
				dialog_lines.push_back("I need a power generator");
			}
			return;
		}

		if (game->generator_placed[game->carried_generator])
		{
			dialog_lines.push_back("This generator is already placed");
			return;
		}

		game->generator_placed[game->carried_generator] = true;
		game->tower_generator[tower_index] = game->carried_generator;
		game->num_generators_placed++;
		game->carried_generator = game_state_t::CARRIED_NONE;
		dialog_lines.push_back("You have placed the generator on the tower");
	}

private:
	int tower_index;
	game_state_t *game;
};
