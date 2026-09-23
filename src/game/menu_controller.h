#pragma once

#include "controller.h"
#include "game_state.h"
#include "menu.h"
#include "../engine/fpg.h"
#include "../engine/pal.h"
#include "../engine/pixmap.h"
#include "../engine/viewport.h"

struct menu_controller_t : public controller_t
{
	enum state_t { MAP1_FADE_IN, MAP1_HOLD, MAP1_FADE_OUT, MAP2_FADE_IN, MENU, START_FADE_OUT };

	static constexpr int HOLD_TICKS = 30;
	static constexpr int FADE_STEPS = 6;

	menu_controller_t(game_state_t *game, pixmap_t *backbuffer, const fpg_t *menu_fpg, viewport_t viewport, texts_t *t)
			: game{game}, backbuffer{backbuffer}, menu_fpg{menu_fpg}, viewport{viewport},
				t{t}, menu{t->get("main_menu_start"), t->get("main_menu_exit")} {}

	game_state_t *game;
	pixmap_t *backbuffer;
	const fpg_t *menu_fpg;
	viewport_t viewport;
	texts_t *t;
	menu_t menu;

	state_t state = MAP1_FADE_IN;
	bool played_intro = false;
	int hold_ticks = 0;

	void reset()
	{
		hold_ticks = 0;
		menu.set_options(t->get("main_menu_start"), t->get("main_menu_exit"));
		menu.reset();
		enter_map(played_intro ? 1 : 0);
	}

	void update(const input_t &input) override
	{
		switch (state)
		{
		case MAP1_FADE_IN:
			draw_map(0);
			pal_update_fade();
			if (!pal_fade_active())
			{
				hold_ticks = 0;
				state = MAP1_HOLD;
			}
			break;

		case MAP1_HOLD:
			draw_map(0);
			if (++hold_ticks >= HOLD_TICKS)
			{
				pal_start_fade(0, 0, 0, FADE_STEPS);
				state = MAP1_FADE_OUT;
			}
			break;

		case MAP1_FADE_OUT:
			draw_map(0);
			pal_update_fade();
			if (!pal_fade_active())
			{
				played_intro = true;
				enter_map(1);
			}
			break;

		case MAP2_FADE_IN:
			draw_map(1);
			pal_update_fade();
			if (!pal_fade_active())
				state = MENU;
			break;

		case MENU:
			draw_map(1);
			draw_menu();
			if (menu.update(input))
			{
				if (menu.selected() == 0)
				{
					pal_start_fade(0, 0, 0, FADE_STEPS);
					state = START_FADE_OUT;
				}
				else
				{
					game->exit_requested = true;
				}
			}
			break;

		case START_FADE_OUT:
			draw_map(1);
			draw_menu();
			pal_update_fade();
			if (!pal_fade_active())
				game->start_game_requested = true;
			break;
		}
	}

private:
	void enter_map(int index)
	{
		pal_set_fade(0, 0, 0);
		pal_start_fade(100, 100, 100, FADE_STEPS);
		state = (index == 0) ? MAP1_FADE_IN : MAP2_FADE_IN;
	}

	void draw_map(int index)
	{
		const pixmap_t *map = menu_fpg->map(size_t(index));
		if (map)
			backbuffer->blit(*map, {0, 0});
	}

	void draw_menu()
	{
		menu.draw(*backbuffer, menu_t::lower_left(viewport));
	}
};
