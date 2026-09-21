#include <iostream>
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
#include "game/menu_controller.h"
#include "game/intro_controller.h"
#include "game/game_controller.h"
#include "game/ending_controller.h"
#include "game/workshop_minigame_controller.h"
#include "game/tower_minigame_controller.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define TARGET_FPS 10

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

	const fpg_t fpg = fpg_t::load("assets/town.fpg", true);
	if (fpg.num_maps() == 0)
	{
		std::cout << "Can't load fpg" << std::endl;
		return -1;
	}

	const fpg_t workshop_fpg = fpg_t::load("assets/workshop.fpg", false);
	if (workshop_fpg.num_maps() < 8)
	{
		std::cout << "Warning: workshop.fpg missing sprites" << std::endl;
	}

	const fpg_t menu_fpg = fpg_t::load("assets/menu.fpg", false);
	if (menu_fpg.num_maps() < 3)
	{
		std::cout << "Warning: menu.fpg missing maps" << std::endl;
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
	auto ending_banner = banner_t{60}; // 6 seconds per page at 10 FPS
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

	int footsteps_voice = -1;
	int humming_voice = -1;

	game_state_t game;
	intro_controller_t intro{&game, &backbuffer};
	game_controller_t game_ctrl{&game, &backbuffer, &player, &banner, &action_text,
														&raycaster, &tilemap, &fpg, VIEWPORT,
														footsteps_sound, humming_sound,
														&footsteps_voice, &humming_voice};
	workshop_minigame_controller_t workshop_minigame{&game, &backbuffer, &workshop_fpg};
	tower_minigame_controller_t tower_minigame{&game, &backbuffer};
	menu_controller_t menu_ctrl{&game, &backbuffer, &menu_fpg, VIEWPORT};
	ending_controller_t ending{&game, &backbuffer, &menu_fpg, &ending_banner, VIEWPORT};
	controller_t *current_controller = &menu_ctrl;
	game.phase = game_state_t::PHASE_MENU;
	menu_ctrl.reset();

	while (screen_update(backbuffer))
	{
		const auto input = input_calculate();

		current_controller->update(input);

		if (game.exit_requested)
			break;

		if (game.start_game_requested)
		{
			game.restart_game();
			game_ctrl.reset();

			tilemap = load_tilemap("assets/town.tma");
			for (uint32_t x = 0; x < tilemap.map_size.x; ++x)
				for (uint32_t y = 0; y < tilemap.map_size.y; ++y)
				{
					raycaster.tile({x, y}, tilemap.tile_at(x, y));
					raycaster.floor({x, y}, tilemap.floor_at(x, y));
				}
		}

		if (game.phase == game_state_t::PHASE_INTRO && current_controller != &intro)
		{
			intro.reset();
			current_controller = &intro;
		}
		else if (game.phase == game_state_t::PHASE_WORKSHOP_MINIGAME && current_controller != &workshop_minigame)
		{
			workshop_minigame.reset();
			current_controller = &workshop_minigame;
		}
		else if (game.phase == game_state_t::PHASE_TOWER_MINIGAME && current_controller != &tower_minigame)
		{
			current_controller = &tower_minigame;
		}
		else if (game.phase == game_state_t::PHASE_MENU && current_controller != &menu_ctrl)
		{
			menu_ctrl.reset();
			current_controller = &menu_ctrl;
		}
		else if (game.phase == game_state_t::PHASE_ENDING && current_controller != &ending)
		{
			ending.reset();
			current_controller = &ending;
		}
		else if (game.phase != game_state_t::PHASE_INTRO
				 && game.phase != game_state_t::PHASE_WORKSHOP_MINIGAME
				 && game.phase != game_state_t::PHASE_TOWER_MINIGAME
				 && game.phase != game_state_t::PHASE_MENU
				 && game.phase != game_state_t::PHASE_ENDING
				 && current_controller != &game_ctrl)
		{
			current_controller = &game_ctrl;
		}
	}

	if (footsteps_sound)
		destroy_sample(footsteps_sound);
	if (humming_sound)
		destroy_sample(humming_sound);
}

END_OF_MAIN();
