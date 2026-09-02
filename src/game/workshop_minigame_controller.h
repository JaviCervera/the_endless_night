#pragma once

#include "controller.h"
#include "game_state.h"
#include "../engine/pixmap.h"

struct workshop_minigame_controller_t : public controller_t
{
	workshop_minigame_controller_t(game_state_t *game, pixmap_t *backbuffer)
			: game{game}, backbuffer{backbuffer} {}

	game_state_t *game;
	pixmap_t *backbuffer;

	void update(const input_t &input) override
	{
		backbuffer->fill(0);
		backbuffer->text("You are in the minigame! - Press X to exit",
						 {40, 90}, 15);

		if (input.exit_minigame)
		{
			game->loop_start_clock = clock() - (clock_t)(game->saved_elapsed_ticks * CLOCKS_PER_SEC / 10);
			game->phase = game_state_t::PHASE_PLAYING;
		}
	}
};