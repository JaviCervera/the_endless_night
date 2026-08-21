#pragma once

#include <cstdio>
#include <allegro.h>
#include "controller.h"
#include "game_state.h"
#include "../engine/pixmap.h"
#include "../engine/pal.h"

struct intro_controller_t : public controller_t
{
	intro_controller_t(game_state_t *game, pixmap_t *backbuffer)
			: game{game}, backbuffer{backbuffer} {}

	game_state_t *game;
	pixmap_t *backbuffer;

	int intro_timer = 0;
	int intro_sub = 0;
	bool first_start = true;

	void reset()
	{
		intro_timer = 0;
		intro_sub = 0;
	}

	void update(const input_t &input) override
	{
		if (intro_sub == 0)
		{
			if (first_start)
			{
				first_start = false;
				intro_timer = 36;
				intro_sub = 2;
			}
			else
			{
				if (!pal_fade_active())
					pal_start_fade(0, 0, 0, 2);
				intro_sub = 1;
			}
		}

		if (intro_sub == 1)
		{
			pal_update_fade();
			if (!pal_fade_active())
			{
				intro_timer = 36;
				pal_start_fade(100, 100, 100, 12);
				intro_sub = 2;
			}
		}

		if (intro_sub == 2)
		{
			backbuffer->fill(0);

			char loop_text[32];
			std::snprintf(loop_text, sizeof(loop_text), "Loop %d", game->num_loop);
			int loop_text_width = text_length(font, loop_text);
			backbuffer->text(loop_text, {static_cast<uint32_t>((backbuffer->size().x - loop_text_width) / 2), 100u}, 15);

			pal_update_fade();

			intro_timer--;
			if (intro_timer <= 0)
			{
				game->phase = game_state_t::PHASE_PLAYING;
			}
		}
	}
};
