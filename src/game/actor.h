#pragma once

#include <string>
#include <vector>
#include "../engine/entity.h"

struct actor_t : public entity_t
{
	std::string action_text;
	real_t action_distance = real_t(1.5f);
	std::vector<std::string> dialog_lines;

	inline static std::vector<actor_t*> all;

	actor_t() {
		all.push_back(this);
	}

	actor_t(const actor_t &) = delete;

	~actor_t() override
	{
		for (auto it = all.begin(); it != all.end(); ++it)
			if (*it == this)
			{
				all.erase(it);
				break;
			}
	}

	actor_t &operator=(const actor_t &) = delete;

	virtual void on_action_pressed()
	{
	}

	virtual bool can_show_action(vec2_t /*player_pos*/) const
	{
		return true;
	}
};
