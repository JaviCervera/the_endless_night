#pragma once

#include "actionable.h"
#include "player.h"

struct crowbar_t : public actionable_t
{
	crowbar_t(player_t &player, vec2_t pos) : actionable_t{CROWBAR_NAME, pos, CROWBAR_ID}, player{&player}
	{
		collidable = false;
		action_text = "Take crowbar";
		if (already_picked)
		{
			this->pos = spawn_pos;
		}
	}

	~crowbar_t() override
	{
		if (was_picked())
			spawn_pos = player->cam.pos;
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
		if (!already_picked)
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
	inline static auto already_picked = false;
	inline static auto spawn_pos = vec2_t{real_t(0.0f), real_t(0.0f)};

	player_t *player;
	int anim_tick = 0;

	void pick()
	{
		already_picked = true;
		active = false;
	}

	bool is_barn_door_opened() const
	{
		const auto d = entity_t::entity_with_name(BARN_DOOR_NAME);
		return d && !d->active;
	}
};
