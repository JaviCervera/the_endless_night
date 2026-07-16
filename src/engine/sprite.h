#pragma once

#include <vector>
#include "vec2.h"

struct sprite_t
{
	vec2_t pos;
	uint8_t fpg_idx;
	bool collidable = true;
	bool halved = false;

	inline static std::vector<const sprite_t*> all;

	sprite_t()
	{
		all.push_back(this);
	}

	sprite_t(const sprite_t &o) : pos(o.pos), fpg_idx(o.fpg_idx), collidable(o.collidable), halved(o.halved)
	{
		all.push_back(this);
	}

	virtual ~sprite_t()
	{
		for (auto it = all.begin(); it != all.end(); ++it)
			if (*it == this)
			{
				all.erase(it);
				break;
			}
	}

	sprite_t &operator=(const sprite_t &) = delete;
};
