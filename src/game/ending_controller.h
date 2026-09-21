#pragma once

#include <allegro.h>
#include "controller.h"
#include "game_state.h"
#include "banner.h"
#include "../engine/fpg.h"
#include "../engine/pal.h"
#include "../engine/pixmap.h"
#include "../engine/viewport.h"

struct ending_controller_t : public controller_t
{
	enum state_t { FADE_IN, BANNER, TITLE, CREDITS };

	static constexpr size_t ENDING_MAP_INDEX = 2;
	static constexpr int FADE_STEPS = 6;
	static constexpr int TITLE_TICKS = 30;   // 3 seconds at 10 FPS
	static constexpr int CREDITS_TICKS = 30; // 3 seconds at 10 FPS

	ending_controller_t(game_state_t *game, pixmap_t *backbuffer, const fpg_t *menu_fpg, banner_t *banner, viewport_t viewport)
			: game{game}, backbuffer{backbuffer}, menu_fpg{menu_fpg}, banner{banner}, viewport{viewport} {}

	game_state_t *game;
	pixmap_t *backbuffer;
	const fpg_t *menu_fpg;
	banner_t *banner;
	viewport_t viewport;

	state_t state = FADE_IN;
	int ticks = 0;

	void reset()
	{
		state = FADE_IN;
		ticks = 0;

		pal_set_fade(0, 0, 0);
		pal_start_fade(100, 100, 100, FADE_STEPS);

		banner->clear();
		banner->show("The electrical discharge destroys the alien spacecraft. At last, you have closed the cycle of...");
	}

	void update(const input_t &input) override
	{
		switch (state)
		{
		case FADE_IN:
			draw_scene();
			pal_update_fade();
			if (!pal_fade_active())
				state = BANNER;
			break;

		case BANNER:
			draw_scene();
			banner->update();
			if (!banner->active())
			{
				ticks = 0;
				state = TITLE;
			}
			break;

		case TITLE:
			draw_centered("THE ENDLESS NIGHT");
			if (++ticks >= TITLE_TICKS)
			{
				ticks = 0;
				state = CREDITS;
			}
			break;

		case CREDITS:
			draw_centered("A game by Javi \"Jedive\" Cervera");
			if (++ticks >= CREDITS_TICKS)
				game->phase = game_state_t::PHASE_MENU;
			break;
		}
	}

private:
	void draw_scene()
	{
		backbuffer->fill(0);

		const pixmap_t *map = menu_fpg->map(ENDING_MAP_INDEX);
		if (map)
			backbuffer->blit(*map, ivec2_t{0, viewport.y});

		banner->draw(*backbuffer);
	}

	void draw_centered(const char *text)
	{
		backbuffer->fill(0);

		const auto size = backbuffer->size();
		const int x = (int(size.x) - text_length(font, text)) / 2;
		const int y = (int(size.y) - text_height(font)) / 2;
		backbuffer->text(text, uvec2_t{uint32_t(x), uint32_t(y)}, 15);
	}
};
