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
#include "game/farmer.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define TARGET_FPS 20

#define FARMER1_ID 7
#define FARMER2_ID 8
#define PLAYER_ID 9

int main()
{
	real_trig_init();
	screen_open("Raycaster", {SCREEN_WIDTH, SCREEN_HEIGHT}, TARGET_FPS);
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

	static constexpr int VP_W = 300;
	static constexpr int VP_H = 120;
	static constexpr int VP_X = (SCREEN_WIDTH - VP_W) / 2;
	static constexpr int VP_Y = (SCREEN_HEIGHT - VP_H) / 2;
	static constexpr viewport_t VIEWPORT{VP_X, VP_Y, VP_W, VP_H};

	auto backbuffer = pixmap_t{{SCREEN_WIDTH, SCREEN_HEIGHT}, 0};
	/*backbuffer.rectfill({0, 0}, {SCREEN_WIDTH, VP_Y - 2}, 18);
	backbuffer.rectfill({0, VP_Y - 2}, {VP_X - 2, VP_H + 4}, 18);
	backbuffer.rectfill({VP_X + VP_W + 2, VP_Y - 2}, {SCREEN_WIDTH - VP_X - VP_W - 2, VP_H + 4}, 18);
	backbuffer.rectfill({0, VP_Y + VP_H + 2}, {SCREEN_WIDTH, SCREEN_HEIGHT - VP_Y - VP_H - 2}, 18);
	backbuffer.rect({2, 2}, {SCREEN_WIDTH - 4, VP_Y - 6}, 28);
	backbuffer.rectfill({3, 3}, {SCREEN_WIDTH - 6, VP_Y - 8}, 2);*/
	// backbuffer.text("Something strange has happened in the", {5, 5}, 15);
	// backbuffer.text("fields.", {5, 15}, 15);
	// backbuffer.text("Let's go take a look.", {5, 25}, 15);

	auto player = player_t{tilemap, real_t(3.0f / TARGET_FPS), real_t(2.0f / TARGET_FPS), real_t(0.25f)};
	auto banner = banner_t{};
	auto action_text = action_text_t{};
	banner.show("Something strange has happened in the fields to the south, but the path is closed.");

	auto raycaster = raycaster_t{{tilemap.map_size.x, tilemap.map_size.y}, fpg};
#if FOG_ENABLED
	raycaster.fog_color = pal_find_closest(0, 0, 0);
	raycaster.fog_start = real_t(0);
	raycaster.fog_end = real_t(8);
#endif
	for (uint32_t x = 0; x < tilemap.map_size.x; ++x)
	{
		for (uint32_t y = 0; y < tilemap.map_size.y; ++y)
		{
			raycaster.tile({x, y}, tilemap.tile_at(x, y));
			raycaster.floor({x, y}, tilemap.floor_at(x, y));
		}
	}

	std::vector<std::unique_ptr<actor_t>> actors;
	for (uint32_t y = 0; y < tilemap.map_size.y; ++y)
		for (uint32_t x = 0; x < tilemap.map_size.x; ++x)
		{
			const uint8_t id = tilemap.entity_at(x, y);
			switch (id)
			{
			case PLAYER_ID:
				player.position(vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)});
				break;
			case FARMER1_ID:
			case FARMER2_ID:
			{
				auto farmer = std::make_unique<farmer_t>();
				farmer->pos = vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)};
				farmer->fpg_idx = uint8_t(id - 1);
				actors.push_back(std::move(farmer));
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

	while (screen_update(backbuffer))
	{
		const auto input = input_calculate();
		player.update(input);
		action_text.update(actors, player.cam.pos, input);
		raycaster.render(player.cam, actors, backbuffer, VIEWPORT);

		banner.update();
		backbuffer.rectfill({0, 0}, {SCREEN_WIDTH, VP_Y - 2}, 0);
		banner.draw(backbuffer);

		backbuffer.rectfill({0, VP_Y + VP_H}, {SCREEN_WIDTH, SCREEN_HEIGHT}, 0);
		action_text.draw(backbuffer);

		// Space: fade to white (200) over 1 second; release: fade back to normal (100).
		static int s_fade = 100;
		static constexpr int FADE_STEP = 100 / TARGET_FPS; // units per frame for 1-second transition
		if (screen_key(SCREEN_KEY_SPACE))
			s_fade = std::min(s_fade + FADE_STEP, 200);
		else
			s_fade = std::max(s_fade - FADE_STEP, 100);
		pal_set_fade(s_fade, s_fade, s_fade);

		// char fps_mgr[16];
		// std::snprintf(fps_mgr, sizeof(fps_mgr), "FPS: %d / %d", screen_current_fps(), screen_target_fps());
		// backbuffer.text(fps_mgr, {VP_X + 4, VP_Y + 4}, 15);
	}
}

END_OF_MAIN();
