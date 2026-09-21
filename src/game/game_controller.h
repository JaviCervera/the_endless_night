#pragma once

#include <cstdio>
#include <ctime>
#include <allegro.h>
#include "controller.h"
#include "game_state.h"
#include "player.h"
#include "banner.h"
#include "action_text.h"
#include "menu.h"
#include "compass.h"
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
#include "fence.h"
#include "farmer.h"
#include "barn_worker.h"
#include "crowbar.h"
#include "generator.h"
#include "barn_door.h"
#include "plant.h"
#include "station_door.h"
#include "station_key.h"
#include "vine.h"
#include "tree_note.h"
#include "workshop_door.h"
#include "radio_station.h"
#include "power_tower.h"

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

	compass_t compass;
	bool started = false;
	int finish_ticks = 0;

	bool paused = false;
	int pause_elapsed_ticks = 0;
	bool exit_to_menu_fade = false;
	menu_t pause_menu{"RESUME GAME", "EXIT TO MENU"};

	enum minigame_fade_state_t { FADE_NONE, FADE_OUT_TO_WORKSHOP_MINIGAME, FADE_IN_FROM_WORKSHOP_MINIGAME, FADE_OUT_TO_TOWER_MINIGAME, FADE_IN_FROM_TOWER_MINIGAME, FADE_OUT_TO_ENDING };
	minigame_fade_state_t minigame_fade_state = FADE_NONE;

	void update(const input_t &input) override
	{
		if (paused)
		{
			if (exit_to_menu_fade)
			{
				pal_update_fade();

				raycaster->render(player->cam, *backbuffer, viewport);
				draw_pause_overlay();

				if (!pal_fade_active())
				{
					stop_footsteps();
					pause_humming();

					game->phase = game_state_t::PHASE_MENU;
					exit_to_menu_fade = false;
					paused = false;
				}
				return;
			}

			if (input.cancel)
			{
				resume();
				return;
			}

			if (pause_menu.update(input))
			{
				if (pause_menu.selected() == 0)
				{
					resume();
					return;
				}

				pal_start_fade(0, 0, 0, 6);
				exit_to_menu_fade = true;
				return;
			}

			raycaster->render(player->cam, *backbuffer, viewport);
			draw_pause_overlay();
			return;
		}

		if (input.cancel && game->phase == game_state_t::PHASE_PLAYING)
		{
			paused = true;
			pause_elapsed_ticks = game->elapsed_ticks();
			return;
		}

		// Handle fade OUT to workshop minigame
		if (game->request_workshop_minigame && minigame_fade_state == FADE_NONE)
		{
			pal_start_fade(0, 0, 0, 4);
			pause_humming();
			minigame_fade_state = FADE_OUT_TO_WORKSHOP_MINIGAME;
			game->request_workshop_minigame = false;
		}

		if (minigame_fade_state == FADE_OUT_TO_WORKSHOP_MINIGAME)
		{
			pal_update_fade();
			if (!pal_fade_active())
			{
				game->phase = game_state_t::PHASE_WORKSHOP_MINIGAME;
				minigame_fade_state = FADE_NONE;
			}
		}

		// Handle fade OUT to tower minigame
		if (game->request_tower_minigame && minigame_fade_state == FADE_NONE)
		{
			pal_start_fade(0, 0, 0, 4);
			pause_humming();
			minigame_fade_state = FADE_OUT_TO_TOWER_MINIGAME;
			game->request_tower_minigame = false;
		}

		if (minigame_fade_state == FADE_OUT_TO_TOWER_MINIGAME)
		{
			pal_update_fade();
			if (!pal_fade_active())
			{
				game->phase = game_state_t::PHASE_TOWER_MINIGAME;
				minigame_fade_state = FADE_NONE;
			}
		}

		// Handle fade IN from workshop minigame
		if (game->returning_from_workshop_minigame && minigame_fade_state == FADE_NONE)
		{
			pal_start_fade(100, 100, 100, 4);
			resume_humming();
			minigame_fade_state = FADE_IN_FROM_WORKSHOP_MINIGAME;
			game->returning_from_workshop_minigame = false;
		}

		if (minigame_fade_state == FADE_IN_FROM_WORKSHOP_MINIGAME)
		{
			pal_update_fade();
			if (!pal_fade_active())
			{
				if (game->workshop_minigame_won)
					banner->show("I got a power generator!");
				else
					banner->show("I should have paid more attention at school");
				game->workshop_minigame_won = false;

				minigame_fade_state = FADE_NONE;
			}
		}

		// Handle fade IN from tower minigame
		if (game->returning_from_tower_minigame && minigame_fade_state == FADE_NONE)
		{
			pal_start_fade(100, 100, 100, 4);
			resume_humming();
			minigame_fade_state = FADE_IN_FROM_TOWER_MINIGAME;
			game->returning_from_tower_minigame = false;
		}

		if (minigame_fade_state == FADE_IN_FROM_TOWER_MINIGAME)
		{
			pal_update_fade();
			if (!pal_fade_active())
			{
				minigame_fade_state = FADE_NONE;
			}
		}

		// Handle fade OUT to ending
		if (game->request_ending && minigame_fade_state == FADE_NONE)
		{
			pal_start_fade(0, 0, 0, 4);
			pause_humming();
			minigame_fade_state = FADE_OUT_TO_ENDING;
			game->request_ending = false;
		}

		if (minigame_fade_state == FADE_OUT_TO_ENDING)
		{
			pal_update_fade();
			if (!pal_fade_active())
			{
				game->phase = game_state_t::PHASE_ENDING;
				minigame_fade_state = FADE_NONE;
			}
		}

		// Safety: never render raycaster in minigame or ending phases
		if (game->phase == game_state_t::PHASE_WORKSHOP_MINIGAME || game->phase == game_state_t::PHASE_TOWER_MINIGAME || game->phase == game_state_t::PHASE_ENDING)
			return;

		if (!started)
		{
			entity_t::clear_all();
			spawn_entities();
			game->loop_start_clock = clock();
			if (game->num_loop == 1)
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

			update_footsteps(input);
			update_humming();

			raycaster->render(player->cam, *backbuffer, viewport);

			draw_hud();

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

			update_footsteps(input);
			update_humming();

			raycaster->render(player->cam, *backbuffer, viewport);

			draw_hud();

			pal_update_fade();

			if (!pal_fade_active())
			{
				entity_t::clear_all();
				banner->reset();

				pal_set_fade(100, 100, 100);

				stop_footsteps();
				pause_humming();

				game->advance_loop();

				started = false;
			}
		}
	}

	void reset()
	{
		started = false;
		paused = false;
		pause_elapsed_ticks = 0;
		exit_to_menu_fade = false;
		finish_ticks = 0;
		minigame_fade_state = FADE_NONE;
		pause_menu.reset();
		banner->reset();
	}

	void pause_humming()
	{
		if (*humming_voice >= 0)
		{
			voice_stop(*humming_voice);
			*humming_voice = -1;
		}
	}

	void resume_humming()
	{
		if (humming_sound)
		{
			*humming_voice = play_sample(humming_sound, 64, 128, humming_sound->freq, 1);
		}
	}

private:
	int loop_elapsed_ticks()
	{
		return int((clock() - game->loop_start_clock) * TARGET_FPS / CLOCKS_PER_SEC);
	}

	void resume()
	{
		paused = false;
		game->loop_start_clock = clock() - (clock_t)(pause_elapsed_ticks * CLOCKS_PER_SEC / TARGET_FPS);
	}

	void draw_pause_overlay()
	{
		const char *msg = "PAUSED";
		const int text_w = text_length(font, msg);
		backbuffer->text(msg, {uint32_t(viewport.x + (viewport.w - text_w) / 2), uint32_t(viewport.y + viewport.h / 2 - 4)}, 15);
		pause_menu.draw(*backbuffer, menu_t::lower_left(viewport));
	}

	void draw_hud()
	{
		int secs_left = (LOOP_FRAMES - loop_elapsed_ticks()) / TARGET_FPS;
		if (secs_left < 0)
			secs_left = 0;

		char dbg[64];
		// std::snprintf(dbg, sizeof(dbg), "%d FPS", screen_current_fps());
		// backbuffer->text(dbg, {uint32_t(viewport.x + 4), uint32_t(viewport.y + 4)}, 15);
		std::snprintf(dbg, sizeof(dbg), "%02d:%02d", secs_left / 60, secs_left % 60);
		backbuffer->text(dbg, {uint32_t(viewport.x + 4), uint32_t(viewport.y + 4)}, 15);

		auto bs = backbuffer->size();
		backbuffer->rectfill({0u, uint32_t(viewport.y + viewport.h)}, {bs.x, bs.y}, 0);
		backbuffer->rectfill({0u, 0u}, {bs.x, uint32_t(viewport.y - 2)}, 0);

		compass.draw(*backbuffer, player->cam, viewport);
	}

	void update_footsteps(const input_t &input)
	{
		const bool moving = !game->player_blocked && input.forward != 0;
		if (moving && footsteps_sound)
		{
			if (*footsteps_voice < 0 || !voice_check(*footsteps_voice))
				*footsteps_voice = play_sample(footsteps_sound, 255, 128, 1000, 0);
		}
		else if (!moving)
		{
			stop_footsteps();
		}
	}

	void stop_footsteps()
	{
		if (*footsteps_voice >= 0)
		{
			voice_stop(*footsteps_voice);
			*footsteps_voice = -1;
		}
	}

	void update_humming()
	{
		if (*humming_voice < 0)
			return;

		int remaining = LOOP_FRAMES - loop_elapsed_ticks();
		int freq = humming_sound->freq;
		if (remaining < 80)
		{
			int t = 80 - remaining;
			freq = freq + t * (freq / 320);
		}
		voice_set_frequency(*humming_voice, freq);
	}

	void spawn_entities()
	{
		int power_tower_count = 0;

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
					new crowbar_t(*player, pos, game);
					break;
				case GENERATOR_ID:
					if (!game->generator_placed[game_state_t::CARRIED_BARN_GENERATOR])
						new generator_t(*player, pos, game);
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
				case FENCE_ID:
					new fence_t(game, pos);
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
				case RADIO_STATION_ID:
					new radio_station_t(pos, game);
					break;
				case POWER_TOWER_ID:
					new power_tower_t(pos, power_tower_count++, game);
					break;
				case TREE_NOTE_ID:
					new tree_note_t(game, pos);
					break;
				case WORKSHOP_DOOR_ID:
					new workshop_door_t(game, pos);
					break;
				default:
					if (id != 0)
						new entity_t("", pos, id);
					break;
				}
			}
	}
};
