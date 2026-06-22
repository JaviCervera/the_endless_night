#pragma once

#include <string>
#include <vector>
#include "../engine/sprite.h"

struct actor_t : public sprite_t
{
	std::string action_text;
	real_t action_distance = real_t(1.5f);
	bool dead = false;
	std::vector<std::string> dialog_lines;

	virtual void on_action_pressed() {}
	virtual bool can_show_action(vec2_t /*player_pos*/) const { return true; }

	real_t distance_sq(vec2_t player_pos) const
	{
		const auto d = pos - player_pos;
		return d.dot(d);
	}
};
