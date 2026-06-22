#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <allegro.h>
#include "actor.h"
#include "input.h"
#include "game_state.h"
#include "../engine/pixmap.h"

struct action_text_t
{
	actor_t *update(const std::vector<std::unique_ptr<actor_t>> &actors, vec2_t player_pos, const input_t &input, game_state_t &game)
	{
		m_text.clear();
		m_target = nullptr;

		if (game.player_blocked)
		{
			m_text = "...";
			return nullptr;
		}

		real_t best_dsq = real_t::large();
		for (auto &actor : actors)
		{
			if (!actor || actor->dead)
				continue;
			if (!actor->can_show_action(player_pos))
				continue;
			const auto dsq = actor->distance_sq(player_pos);
			const auto max_dsq = actor->action_distance * actor->action_distance;
			if (dsq <= max_dsq && dsq < best_dsq)
			{
				best_dsq = dsq;
				m_text = actor->action_text;
				m_target = actor.get();
			}
		}

		if (input.action && m_target)
		{
			m_target->on_action_pressed();
			return m_target;
		}

		return nullptr;
	}

	void draw(pixmap_t &backbuffer) const
	{
		if (m_text.empty())
			return;

		int text_w = text_length(font, m_text.c_str());
		int x = std::max(0, (320 - text_w) / 2);
		backbuffer.text(m_text.c_str(), {static_cast<uint32_t>(x), 190u}, 15);
	}

	const std::string &text() const { return m_text; }

private:
	std::string m_text;
	actor_t *m_target = nullptr;
};
