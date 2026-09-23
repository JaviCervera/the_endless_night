#pragma once

#include <allegro.h>
#include "controller.h"
#include "game_state.h"
#include "menu.h"
#include "../engine/pal.h"
#include "../engine/pixmap.h"
#include "../engine/texts.h"
#include "../engine/viewport.h"

struct lang_controller_t : public controller_t
{
	static constexpr int FADE_STEPS = 6;
	static constexpr const char *EN_FILE = "assets/lang/en.ini";
	static constexpr const char *ES_FILE = "assets/lang/es.ini";

	lang_controller_t(game_state_t *game, pixmap_t *backbuffer, viewport_t viewport, texts_t *t)
			: game{game}, backbuffer{backbuffer}, viewport{viewport}, t{t},
				menu{"English", "Espa\xA4ol"} {} // \xA4 is 'ñ' in CP 437

	void reset()
	{
		menu.reset();
		load_failed = false;
		pal_set_fade(0, 0, 0);
		pal_start_fade(100, 100, 100, FADE_STEPS);
	}

	void update(const input_t &input) override
	{
		if (menu.update(input))
		{
			load_failed = false;
			t->load(menu.selected() == 0 ? EN_FILE : ES_FILE);
			if (t->loaded())
			{
				backbuffer->fill(0);
				game->phase = game_state_t::PHASE_MENU;
				return;
			}

			t->load(EN_FILE);
			load_failed = true;
		}

		backbuffer->fill(0);

		if (load_failed)
		{
			const char *msg = "Can't load language file";
			const int msg_w = text_length(font, msg);
			backbuffer->text(msg, {uint32_t((backbuffer->size().x - msg_w) / 2), 20u}, 15);
		}

		menu.draw(*backbuffer, menu_t::lower_left(viewport));
		pal_update_fade();
	}

	game_state_t *game;
	pixmap_t *backbuffer;
	viewport_t viewport;
	texts_t *t;
	menu_t menu;
	bool load_failed = false;
};
