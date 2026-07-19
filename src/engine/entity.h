#pragma once

#include <vector>
#include "vec2.h"

struct entity_t
{
	std::string name;
	bool active = true;
	vec2_t pos;
	uint8_t fpg_id;
	bool collidable = true;
	bool halved = false;

	entity_t(std::string name, vec2_t pos, uint8_t fpg_id) : name(name), pos(pos), fpg_id(fpg_id)
	{
		all.push_back(this);
	}

	entity_t(const entity_t &o)
			: name(o.name),
				pos(o.pos),
				fpg_id(o.fpg_id),
				collidable(o.collidable),
				halved(o.halved)
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
			if (dx * dx + dy * dy < (ent->halved ? real_t(0.065f) : real_t(0.26f)))
				return ent;
		}
		return nullptr;
	}

	static size_t num_entities_with_name(const std::string &name)
	{
		size_t count = 0;
		for (auto *ent : all)
		{
			if (ent->name == name)
				count++;
		}
		return count;
	}

	static entity_t *entity_with_name(const std::string &name, size_t index = 0)
	{
		size_t current_index = 0;
		for (auto *ent : all)
		{
			if (ent->name == name)
			{
				if (current_index == index)
					return ent;
				current_index++;
			}
		}
		return nullptr;
	}

private:
	inline static std::vector<entity_t *> all;
};
