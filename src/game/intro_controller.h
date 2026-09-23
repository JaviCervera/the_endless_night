#pragma once

#include <allegro.h>
#include "controller.h"
#include "game_state.h"
#include "../engine/pixmap.h"
#include "../engine/pal.h"
#include "../engine/texts.h"

struct intro_controller_t : public controller_t
{
	intro_controller_t(game_state_t *game, pixmap_t *backbuffer, texts_t *t)
			: game{game}, backbuffer{backbuffer}, t{t} {}

	game_state_t *game;
	pixmap_t *backbuffer;
	texts_t *t;

	int intro_timer = 0;
	int intro_sub = 0;

	void reset()
	{
		intro_timer = 0;
		intro_sub = 0;
	}

	void update(const input_t &input) override
	{
		if (intro_sub == 0)
		{
			if (!pal_fade_active())
				pal_start_fade(0, 0, 0, 2);
			intro_sub = 1;
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

			const auto loop_text = t->format("loop_banner", game->num_loop);
			const int loop_text_width = text_length(font, loop_text.c_str());
			backbuffer->text(loop_text.c_str(), {static_cast<uint32_t>((backbuffer->size().x - loop_text_width) / 2), 100u}, 15);

			pal_update_fade();

			intro_timer--;
			if (intro_timer <= 0)
			{
				game->phase = game_state_t::PHASE_PLAYING;
			}
		}
	}
};
