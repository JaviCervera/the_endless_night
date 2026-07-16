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

	virtual void update()
	{
	}

	real_t distance_sq(vec2_t point) const
	{
		const auto d = pos - point;
		return d.dot(d);
	}

	static void update_all()
	{
		for (auto *entity : all)
			if (entity->active)
				entity->update();
	}

	static void clear_all()
	{
		while (!all.empty())
			delete all.front();
	}

	static size_t num_entities()
	{
		return all.size();
	}

	static entity_t *get_entity(size_t index)
	{
		return (index < all.size()) ? all[index] : nullptr;
	}

	static entity_t *entity_at(vec2_t point)
	{
		for (auto *ent : all)
		{
			if (!ent->collidable)
				continue;
			const auto dx = point.x - ent->pos.x;
			const auto dy = point.y - ent->pos.y;
			if (dx*dx + dy*dy < (ent->halved ? real_t(0.065f) : real_t(0.26f)))
				return ent;
		}
		return nullptr;
	}

private:
	inline static std::vector<entity_t*> all;
};
