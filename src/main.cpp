#include <cstdio>
#include <iostream>
#include <memory>
#include <allegro.h>
#include "engine/fpg.h"
#include "engine/pal.h"
#include "engine/pixmap.h"
#include "engine/screen.h"
#include "engine/tilemap.h"
#include "engine/viewport.h"
#include "engine/raycaster.h"
#include "game/input.h"
#include "game/player.h"
#include "game/banner.h"
#include "game/action_text.h"
#include "game/game_state.h"
#include "game/farmer.h"
#include "game/barn_worker.h"
#include "game/crowbar.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define TARGET_FPS 20

#define CROWBAR_ID 6
#define TREE_ID 11
#define FARMER1_ID 8
#define FARMER2_ID 9
#define PLAYER_ID 10
#define BARN_DOOR_ID 12

#define LOOP_FRAMES 720

static game_state_t g_game;

static int loop_elapsed_ticks()
{
	return int((clock() - g_game.loop_start_clock) * TARGET_FPS / CLOCKS_PER_SEC);
}

static const char *state_names[] = {
	"",
	"The Awakening",
	"Collect Parts",
};

static void spawn_entities(const tilemap_t &tilemap, const fpg_t &fpg,
													 std::vector<std::unique_ptr<actor_t>> &actors,
													 player_t &player)
{
	actors.clear();
	g_game.crowbar_alive = false;
	g_game.crowbar_ptr = nullptr;

	for (uint32_t y = 0; y < tilemap.map_size.y; ++y)
		for (uint32_t x = 0; x < tilemap.map_size.x; ++x)
		{
			const uint8_t id = tilemap.entity_at(x, y);
			switch (id)
			{
			case PLAYER_ID:
			{
				player.position(vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)});
				player.cam.dir = vec2_t{real_t(0.0f), real_t(1.0f)};
				player.cam.plane = vec2_t{real_t(0.66f), real_t(0.0f)};
				break;
			}
			case FARMER1_ID:
			{
				auto farmer = std::make_unique<farmer_t>(&g_game);
				farmer->pos = vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)};
				farmer->fpg_idx = uint8_t(id - 1);
				actors.push_back(std::move(farmer));
				break;
			}
			case FARMER2_ID:
			{
				auto worker = std::make_unique<barn_worker_t>(&g_game);
				worker->pos = vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)};
				worker->fpg_idx = uint8_t(id - 1);
				actors.push_back(std::move(worker));
				break;
			}
			case CROWBAR_ID:
			{
				if (g_game.adventure_state == 1)
				{
					auto crowbar = std::make_unique<crowbar_t>(&g_game);
					crowbar->pos = vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)};
					if (g_game.player_last_picked_object == 1 && g_game.num_loop_in_state > 1)
						crowbar->pos = g_game.player_end_pos;
					g_game.crowbar_ptr = crowbar.get();
					g_game.crowbar_alive = true;
					actors.push_back(std::move(crowbar));
				}
				break;
			}
			default:
				if (id != 0)
				{
					auto actor = std::make_unique<actor_t>();
					actor->pos = vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)};
					actor->fpg_idx = uint8_t(id - 1);
					actors.push_back(std::move(actor));
				}
				break;
			}
		}
}

int main()
{
	real_trig_init();
	screen_open("The Endless Night", {SCREEN_WIDTH, SCREEN_HEIGHT}, TARGET_FPS);
	atexit(screen_close);

	if (!pal_load("assets/div.pal"))
	{
		std::cout << "Can't load palette" << std::endl;
		return -1;
	}

	const fpg_t fpg = fpg_t::load("assets/town.fpg");
	if (fpg.num_maps() == 0)
	{
		std::cout << "Can't load fpg" << std::endl;
		return -1;
	}

	auto tilemap = load_tilemap("assets/town.tma");
	if (tilemap.tiles.empty())
	{
		std::cout << "Can't load tilemap" << std::endl;
		return -1;
	}

	SAMPLE *footsteps_sound = nullptr;
	SAMPLE *humming_sound = nullptr;
	if (install_sound(DIGI_AUTODETECT, MIDI_NONE, nullptr) == 0)
	{
		footsteps_sound = load_wav("assets/steps.wav");
		humming_sound = load_wav("assets/humming.wav");
	}
	else
	{
		std::cout << "Warning: sound init failed" << std::endl;
	}

	static constexpr int VP_W = 300;
	static constexpr int VP_H = 120;
	static constexpr int VP_X = (SCREEN_WIDTH - VP_W) / 2;
	static constexpr int VP_Y = (SCREEN_HEIGHT - VP_H) / 2;
	static constexpr viewport_t VIEWPORT{VP_X, VP_Y, VP_W, VP_H};

	auto backbuffer = pixmap_t{{SCREEN_WIDTH, SCREEN_HEIGHT}, 0};

	auto player = player_t{tilemap, real_t(3.0f / TARGET_FPS), real_t(2.0f / TARGET_FPS), real_t(0.25f)};
	auto banner = banner_t{};
	banner.game = &g_game;
	auto action_text = action_text_t{};

	auto raycaster = raycaster_t{{tilemap.map_size.x, tilemap.map_size.y}, fpg};
#if FOG_ENABLED
	raycaster.fog_color = pal_find_closest(0, 0, 0);
	raycaster.fog_start = real_t(0);
	raycaster.fog_end = real_t(8);
#endif
	for (uint32_t x = 0; x < tilemap.map_size.x; ++x)
		for (uint32_t y = 0; y < tilemap.map_size.y; ++y)
		{
			raycaster.tile({x, y}, tilemap.tile_at(x, y));
			raycaster.floor({x, y}, tilemap.floor_at(x, y));
		}

	std::vector<std::unique_ptr<actor_t>> actors;

	int footsteps_voice = -1;
	int humming_voice = -1;
	bool first_start = true;

	while (screen_update(backbuffer))
	{
		const auto input = input_calculate();

		if (g_game.phase == game_state_t::PHASE_INTRO)
		{
			if (g_game.intro_sub == 0)
			{
				if (first_start)
				{
					first_start = false;
					g_game.intro_timer = 36;
					g_game.intro_sub = 2;
				}
				else
				{
					if (!pal_fade_active())
						pal_start_fade(0, 0, 0, 2);
					g_game.intro_sub = 1;
				}
			}

			if (g_game.intro_sub == 1)
			{
				pal_update_fade();
				if (!pal_fade_active())
				{
					g_game.intro_timer = 36;
					pal_start_fade(100, 100, 100, 12);
					g_game.intro_sub = 2;
				}
			}

			if (g_game.intro_sub == 2)
			{
				backbuffer.fill(0);

				char buf[64];
				int si = g_game.adventure_state;
				const char *sn = (si >= 0 && si < 3) ? state_names[si] : "";
				std::snprintf(buf, sizeof(buf), "CHAPTER %d: %s", si, sn);
				int tw = text_length(font, buf);
				backbuffer.text(buf, {static_cast<uint32_t>((SCREEN_WIDTH - tw) / 2), 100u}, 15);

				if (g_game.num_loop_in_state > 1 && g_game.intro_timer <= 24)
				{
					char lb[32];
					std::snprintf(lb, sizeof(lb), "Loop %d", g_game.num_loop_in_state);
					int lw = text_length(font, lb);
					backbuffer.text(lb, {static_cast<uint32_t>((SCREEN_WIDTH - lw) / 2), 115u}, 15);
				}

				pal_update_fade();

				g_game.intro_timer--;
				if (g_game.intro_timer <= 0)
				{
					g_game.intro_sub = 3;
				}
			}

			if (g_game.intro_sub == 3)
			{
				backbuffer.fill(0);
				pal_update_fade();
				if (!pal_fade_active())
				{
					spawn_entities(tilemap, fpg, actors, player);
					g_game.loop_start_clock = clock();

					if (g_game.adventure_state == 1 && g_game.num_loop_in_state == 1)
						banner.show("What was that sound? It came from outside...");

					g_game.phase = game_state_t::PHASE_PLAYING;

					if (humming_sound)
						humming_voice = play_sample(humming_sound, 64, 128, humming_sound->freq, 1);
				}
			}
		}
		else if (g_game.phase == game_state_t::PHASE_PLAYING)
		{
			player.update(input, g_game);

			if (g_game.crowbar_ptr)
				g_game.crowbar_ptr->animate();

			auto activated = action_text.update(actors, player.cam.pos, input, g_game);
			if (activated)
			{
				std::string combined;
				for (auto &line : activated->dialog_lines)
				{
					if (!combined.empty()) combined += " ";
					combined += line;
				}
				if (!combined.empty())
					banner.show(combined);
				activated->dialog_lines.clear();
			}

			banner.update();

			bool moving = !g_game.player_blocked && input.forward != 0;
			if (moving && footsteps_sound)
			{
				if (footsteps_voice < 0 || !voice_check(footsteps_voice))
					footsteps_voice = play_sample(footsteps_sound, 255, 128, 1000, 0);
			}
			else if (!moving && footsteps_voice >= 0)
			{
				voice_stop(footsteps_voice);
				footsteps_voice = -1;
			}

			if (humming_voice >= 0)
			{
				int remaining = LOOP_FRAMES - loop_elapsed_ticks();
				int freq = humming_sound->freq;
				if (remaining < 80)
				{
					int t = 80 - remaining;
					freq = freq + t * (freq / 320);
				}
				voice_set_frequency(humming_voice, freq);
			}

			/*
			{
				real_t fog_angle = real_t(g_game.loop_ticks) * real_t(0.08727f);
				auto s = real_sin(fog_angle);
				int green = int(real_abs(s) * real_t(80));
				raycaster.ceiling_color = pal_find_closest(0, uint8_t(green * 64 / 100), 0);
			}
			*/

			for (auto it = actors.begin(); it != actors.end();)
			{
				if ((*it)->dead)
				{
					if (it->get() == g_game.crowbar_ptr)
						g_game.crowbar_ptr = nullptr;
					it = actors.erase(it);
				}
				else
					++it;
			}

			raycaster.render(player.cam, actors, backbuffer, VIEWPORT);

			{
				int secs_left = (LOOP_FRAMES - loop_elapsed_ticks()) / TARGET_FPS;
				if (secs_left < 0) secs_left = 0;
				char dbg[64];
				std::snprintf(dbg, sizeof(dbg), "%d FPS", screen_current_fps());
				backbuffer.text(dbg, {uint32_t(VP_X + 4), uint32_t(VP_Y + 4)}, 15);
				std::snprintf(dbg, sizeof(dbg), "%02d:%02d", secs_left / 60, secs_left % 60);
				backbuffer.text(dbg, {uint32_t(VP_X + 4), uint32_t(VP_Y + 14)}, 15);
			}

			backbuffer.rectfill({0, VP_Y + VP_H}, {SCREEN_WIDTH, SCREEN_HEIGHT}, 0);
			backbuffer.rectfill({0, 0}, {SCREEN_WIDTH, VP_Y - 2}, 0);

			banner.draw(backbuffer);
			action_text.draw(backbuffer);

			if (loop_elapsed_ticks() >= LOOP_FRAMES)
			{
				g_game.phase = game_state_t::PHASE_FINISHING;
				g_game.finish_ticks = 0;
			}
		}
		else if (g_game.phase == game_state_t::PHASE_FINISHING)
		{
			if (g_game.finish_ticks == 0)
			{
				actors.clear();
				g_game.crowbar_ptr = nullptr;
				banner.reset();

				pal_start_fade(200, 200, 200, 20);
			}

			g_game.finish_ticks++;

			player.update(input, g_game);

			bool moving = !g_game.player_blocked && input.forward != 0;
			if (moving && footsteps_sound)
			{
				if (footsteps_voice < 0 || !voice_check(footsteps_voice))
					footsteps_voice = play_sample(footsteps_sound, 255, 128, 1000, 0);
			}
			else if (!moving && footsteps_voice >= 0)
			{
				voice_stop(footsteps_voice);
				footsteps_voice = -1;
			}

			if (humming_voice >= 0)
			{
				int remaining = LOOP_FRAMES - loop_elapsed_ticks();
				int freq = humming_sound->freq;
				if (remaining < 80)
				{
					int t = 80 - remaining;
					freq = freq + t * (freq / 320);
				}
				voice_set_frequency(humming_voice, freq);
			}

			raycaster.render(player.cam, actors, backbuffer, VIEWPORT);

			{
				char dbg[64];
				std::snprintf(dbg, sizeof(dbg), "%d FPS", screen_current_fps());
				backbuffer.text(dbg, {uint32_t(VP_X + 4), uint32_t(VP_Y + 4)}, 15);
				std::snprintf(dbg, sizeof(dbg), "00:00");
				backbuffer.text(dbg, {uint32_t(VP_X + 4), uint32_t(VP_Y + 14)}, 15);
			}

			backbuffer.rectfill({0, VP_Y + VP_H}, {SCREEN_WIDTH, SCREEN_HEIGHT}, 0);
			backbuffer.rectfill({0, 0}, {SCREEN_WIDTH, VP_Y - 2}, 0);

			pal_update_fade();

			if (!pal_fade_active())
			{
				pal_set_fade(100, 100, 100);

				g_game.player_end_pos = player.cam.pos;

				if (footsteps_voice >= 0)
				{
					voice_stop(footsteps_voice);
					footsteps_voice = -1;
				}
				if (humming_voice >= 0)
				{
					voice_stop(humming_voice);
					humming_voice = -1;
				}

				g_game.num_loop_in_state++;
				g_game.loop_start_clock = clock();

				g_game.phase = game_state_t::PHASE_INTRO;
				g_game.intro_sub = 0;
				g_game.intro_timer = 0;
			}
		}
	}

	if (footsteps_sound)
		destroy_sample(footsteps_sound);
	if (humming_sound)
		destroy_sample(humming_sound);
}

END_OF_MAIN();
