#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <allegro.h>
#include "actionable.h"
#include "input.h"
#include "game_state.h"
#include "../engine/pixmap.h"

struct action_text_t
{
	explicit action_text_t(texts_t *t) : t{t} {}

	actionable_t *update(vec2_t player_pos, const input_t &input, game_state_t &game)
	{
		m_text.clear();
		m_target = nullptr;

		if (game.player_blocked)
		{
			m_text = t->get("action_blocked");
			return nullptr;
		}

		real_t best_dsq = real_t::large();
		for (auto &actionable : actionable_t::all)
		{
			if (!actionable || !actionable->active)
				continue;
			if (!actionable->can_show_action(player_pos))
				continue;
			const auto dsq = actionable->distance_sq(player_pos);
			const auto max_dsq = actionable->action_distance * actionable->action_distance;
			if (dsq <= max_dsq && dsq < best_dsq)
			{
				best_dsq = dsq;
				m_text = actionable->action_text;
				m_target = actionable;
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
	texts_t *t;
	std::string m_text;
	actionable_t *m_target = nullptr;
};
