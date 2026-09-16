#pragma once

#include "actionable.h"
#include "player.h"
#include "game_state.h"

struct crowbar_t : public actionable_t
{
	crowbar_t(player_t &player, vec2_t pos, game_state_t *game) : actionable_t{CROWBAR_NAME, pos, CROWBAR_ID}, player{&player}, game{game}
	{
		collidable = false;
		action_text = "Take crowbar";
		if (game->crowbar_picked)
		{
			this->pos = game->crowbar_spawn_pos;
		}
	}

	~crowbar_t() override
	{
		if (was_picked())
			game->crowbar_spawn_pos = player->cam.pos;
	}

	void update() override
	{
		if (is_barn_door_opened())
		{
			active = false;
			return;
		}

		anim_tick++;
		if (anim_tick >= 3)
		{
			anim_tick = 0;
			fpg_id = (fpg_id == CROWBAR_ID) ? (CROWBAR_ID + 1) : CROWBAR_ID;
		}
	}

	void on_action_pressed() override
	{
		if (!game->crowbar_picked)
			dialog_lines.push_back("With this I should be able to open the barn.");
		else
			dialog_lines.push_back("The crowbar is where I left it!");
		pick();
	}

	bool was_picked() const
	{
		return !active;
	}

private:
	player_t *player;
	game_state_t *game;
	int anim_tick = 0;

	void pick()
	{
		game->crowbar_picked = true;
		active = false;
	}

	bool is_barn_door_opened() const
	{
		const auto d = entity_t::entity_with_name(BARN_DOOR_NAME);
		return d && !d->active;
	}
};
