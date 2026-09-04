#pragma once

#include "controller.h"
#include "game_state.h"
#include "../engine/pixmap.h"
#include "../engine/pal.h"

struct workshop_minigame_controller_t : public controller_t
{
	enum state_t { FADE_IN, RUNNING, FADE_OUT };

	workshop_minigame_controller_t(game_state_t *game, pixmap_t *backbuffer)
			: game{game}, backbuffer{backbuffer}, state{FADE_IN}
	{
	}

	game_state_t *game;
	pixmap_t *backbuffer;
	state_t state;

	void update(const input_t &input) override
	{
		if (state == FADE_IN)
		{
			if (!pal_fade_active())
				pal_start_fade(100, 100, 100, 4);
			
			if (!pal_update_fade())
				state = RUNNING;
			// Fall through to draw minigame screen during fade-in
		}
		else if (state == FADE_OUT)
		{
			if (!pal_update_fade())
			{
				game->loop_start_clock = clock() - (clock_t)(game->saved_elapsed_ticks * CLOCKS_PER_SEC / 10);
				game->returning_from_workshop_minigame = true;
				game->phase = game_state_t::PHASE_PLAYING;
				state = FADE_IN;
			}
			return;  // Don't draw during fade-out (screen going to black)
		}

		backbuffer->fill(0);
		backbuffer->text("You are in the minigame! - Press X to exit",
						 {40, 90}, 15);

		if (input.exit_minigame)
		{
			pal_start_fade(0, 0, 0, 4);
			state = FADE_OUT;
		}
	}
};