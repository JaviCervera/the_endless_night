#pragma once

#include <cstdio>
#include <ctime>
#include <allegro.h>
#include "controller.h"
#include "game_state.h"
#include "player.h"
#include "banner.h"
#include "action_text.h"
#include "../engine/pixmap.h"
#include "../engine/raycaster.h"
#include "../engine/tilemap.h"
#include "../engine/fpg.h"
#include "../engine/viewport.h"
#include "../engine/pal.h"
#include "../engine/screen.h"
#include "../engine/entity.h"
#include "herbicide.h"
#include "entity_ids.h"
#include "farmer.h"
#include "barn_worker.h"
#include "crowbar.h"
#include "generator.h"
#include "barn_door.h"
#include "plant.h"
#include "station_door.h"
#include "station_key.h"
#include "vine.h"

#define LOOP_FRAMES 300
#define TARGET_FPS 10

struct game_controller_t : public controller_t
{
	game_controller_t(game_state_t *game, pixmap_t *backbuffer,
										player_t *player, banner_t *banner, action_text_t *action_text,
										raycaster_t *raycaster, tilemap_t *tilemap, const fpg_t *fpg,
										viewport_t viewport,
										SAMPLE *footsteps_sound, SAMPLE *humming_sound,
										int *footsteps_voice, int *humming_voice)
			: game{game}, backbuffer{backbuffer}, player{player},
				banner{banner}, action_text{action_text},
				raycaster{raycaster}, tilemap{tilemap}, fpg{fpg},
				viewport{viewport}, footsteps_sound{footsteps_sound},
				humming_sound{humming_sound}, footsteps_voice{footsteps_voice},
				humming_voice{humming_voice} {}

	game_state_t *game;
	pixmap_t *backbuffer;
	player_t *player;
	banner_t *banner;
	action_text_t *action_text;
	raycaster_t *raycaster;
	tilemap_t *tilemap;
	const fpg_t *fpg;
	viewport_t viewport;
	SAMPLE *footsteps_sound;
	SAMPLE *humming_sound;
	int *footsteps_voice;
	int *humming_voice;

	bool started = false;
	int finish_ticks = 0;

	void update(const input_t &input) override
	{
		if (!started)
		{
			entity_t::clear_all();
			spawn_entities();
			game->loop_start_clock = clock();
			if (game->adventure_state == 1 && game->num_loop_in_state == 1)
				banner->show("What was that sound? It came from outside...");
			if (humming_sound)
				*humming_voice = play_sample(humming_sound, 64, 128, humming_sound->freq, 1);
			started = true;
		}

		if (game->phase == game_state_t::PHASE_PLAYING)
		{
			player->update(input, *game);
			entity_t::update_all();

			auto activated = action_text->update(player->cam.pos, input, *game);
			if (activated)
			{
				std::string combined;
				for (auto &line : activated->dialog_lines)
				{
					if (!combined.empty())
						combined += " ";
					combined += line;
				}
				if (!combined.empty())
					banner->show(combined);
				activated->dialog_lines.clear();
			}

			banner->update();

			bool moving = !game->player_blocked && input.forward != 0;
			if (moving && footsteps_sound)
			{
				if (*footsteps_voice < 0 || !voice_check(*footsteps_voice))
					*footsteps_voice = play_sample(footsteps_sound, 255, 128, 1000, 0);
			}
			else if (!moving && *footsteps_voice >= 0)
			{
				voice_stop(*footsteps_voice);
				*footsteps_voice = -1;
			}

			if (*humming_voice >= 0)
			{
				int remaining = LOOP_FRAMES - loop_elapsed_ticks();
				int freq = humming_sound->freq;
				if (remaining < 80)
				{
					int t = 80 - remaining;
					freq = freq + t * (freq / 320);
				}
				voice_set_frequency(*humming_voice, freq);
			}

			raycaster->render(player->cam, *backbuffer, viewport);

			{
				int secs_left = (LOOP_FRAMES - loop_elapsed_ticks()) / TARGET_FPS;
				if (secs_left < 0)
					secs_left = 0;
				char dbg[64];
				std::snprintf(dbg, sizeof(dbg), "%d FPS", screen_current_fps());
				backbuffer->text(dbg, {uint32_t(viewport.x + 4), uint32_t(viewport.y + 4)}, 15);
				std::snprintf(dbg, sizeof(dbg), "%02d:%02d", secs_left / 60, secs_left % 60);
				backbuffer->text(dbg, {uint32_t(viewport.x + 4), uint32_t(viewport.y + 14)}, 15);
			}

			{
				auto bs = backbuffer->size();
				backbuffer->rectfill({0u, uint32_t(viewport.y + viewport.h)}, {bs.x, bs.y}, 0);
				backbuffer->rectfill({0u, 0u}, {bs.x, uint32_t(viewport.y - 2)}, 0);
			}

			banner->draw(*backbuffer);
			action_text->draw(*backbuffer);

			if (loop_elapsed_ticks() >= LOOP_FRAMES)
			{
				game->phase = game_state_t::PHASE_FINISHING;
				finish_ticks = 0;
			}
		}
		else if (game->phase == game_state_t::PHASE_FINISHING)
		{
			if (finish_ticks == 0)
			{
				pal_start_fade(200, 200, 200, 20);
			}

			finish_ticks++;

			player->update(input, *game);

			bool moving = !game->player_blocked && input.forward != 0;
			if (moving && footsteps_sound)
			{
				if (*footsteps_voice < 0 || !voice_check(*footsteps_voice))
					*footsteps_voice = play_sample(footsteps_sound, 255, 128, 1000, 0);
			}
			else if (!moving && *footsteps_voice >= 0)
			{
				voice_stop(*footsteps_voice);
				*footsteps_voice = -1;
			}

			if (*humming_voice >= 0)
			{
				int remaining = LOOP_FRAMES - loop_elapsed_ticks();
				int freq = humming_sound->freq;
				if (remaining < 80)
				{
					int t = 80 - remaining;
					freq = freq + t * (freq / 320);
				}
				voice_set_frequency(*humming_voice, freq);
			}

			raycaster->render(player->cam, *backbuffer, viewport);

			{
				char dbg[64];
				std::snprintf(dbg, sizeof(dbg), "%d FPS", screen_current_fps());
				backbuffer->text(dbg, {uint32_t(viewport.x + 4), uint32_t(viewport.y + 4)}, 15);
				std::snprintf(dbg, sizeof(dbg), "00:00");
				backbuffer->text(dbg, {uint32_t(viewport.x + 4), uint32_t(viewport.y + 14)}, 15);
			}

			{
				auto bs = backbuffer->size();
				backbuffer->rectfill({0u, uint32_t(viewport.y + viewport.h)}, {bs.x, bs.y}, 0);
				backbuffer->rectfill({0u, 0u}, {bs.x, uint32_t(viewport.y - 2)}, 0);
			}

			pal_update_fade();

			if (!pal_fade_active())
			{
				entity_t::clear_all();
				banner->reset();

				pal_set_fade(100, 100, 100);

				if (*footsteps_voice >= 0)
				{
					voice_stop(*footsteps_voice);
					*footsteps_voice = -1;
				}
				if (*humming_voice >= 0)
				{
					voice_stop(*humming_voice);
					*humming_voice = -1;
				}

				game->num_loop_in_state++;
				game->loop_start_clock = clock();

				game->phase = game_state_t::PHASE_INTRO;
				started = false;
			}
		}
	}

private:
	int loop_elapsed_ticks()
	{
		return int((clock() - game->loop_start_clock) * TARGET_FPS / CLOCKS_PER_SEC);
	}

	void spawn_entities()
	{
		game->herbicide_held = false;
		game->station_key_held = false;

		for (uint32_t y = 0; y < tilemap->map_size.y; ++y)
			for (uint32_t x = 0; x < tilemap->map_size.x; ++x)
			{
				const uint8_t id = tilemap->entity_at(x, y);
				const vec2_t pos = vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)};
				switch (id)
				{
				case PLAYER_ID:
					player->position(pos);
					player->cam.dir = vec2_t{real_t(0.0f), real_t(1.0f)};
					player->cam.plane = vec2_t{real_t(0.66f), real_t(0.0f)};
					break;
				case FARMER_ID:
					new farmer_t(game, pos);
					break;
				case BARN_WORKER_ID:
					new barn_worker_t(game, pos);
					break;
				case CROWBAR_ID:
					new crowbar_t(*player, pos);
					break;
				case GENERATOR_ID:
					new generator_t(*player, pos);
					break;
				case BARN_DOOR_ID:
					new barn_door_t(game, tilemap, raycaster, x, y);
					break;
				case PLANT_ID:
					new plant_t(pos);
					break;
				case HERBICIDE_ID:
					new herbicide_t(game, pos);
					break;
				case STATION_DOOR_ID:
					new station_door_t(game, pos);
					break;
				case KEY_ID:
					new station_key_t(game, pos);
					break;
				case VINE_ID:
					new vine_t(game, pos);
					break;
				default:
					if (id != 0)
						new entity_t("", pos, id);
					break;
				}
			}
	}
};
