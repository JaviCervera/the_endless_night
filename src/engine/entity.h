#pragma once

#include <vector>
#include "vec2.h"

struct entity_t
{
	bool active = true;
	vec2_t pos;
	uint8_t fpg_idx;
	bool collidable = true;
	bool halved = false;

	inline static std::vector<const entity_t*> all;

	entity_t()
	{
		all.push_back(this);
	}

	entity_t(const entity_t &o) : pos(o.pos), fpg_idx(o.fpg_idx), collidable(o.collidable), halved(o.halved)
	{
		all.push_back(this);
	}

	virtual ~entity_t()
	{
		for (auto it = all.begin(); it != all.end(); ++it)
			if (*it == this)
			{
				all.erase(it);
				break;
			}
	}

	entity_t &operator=(const entity_t &) = delete;
};
