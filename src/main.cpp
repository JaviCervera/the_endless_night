#include <iostream>
#include <allegro.h>
#include "engine/camera.h"
#include "engine/fpg.h"
#include "engine/pal.h"
#include "engine/pixmap.h"
#include "engine/screen.h"
#include "engine/tilemap.h"
#include "engine/viewport.h"
#include "engine/raycaster.h"
#include "game/player.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define TARGET_FPS 20
#define PLAYER_ID 9

static tilemap_t tilemap;

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

	tilemap = load_tilemap("assets/town.tma");
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
	vec2_t player_pos{real_t(0.5f), real_t(0.5f)};
	for (uint32_t y = 0; y < tilemap.map_size.y; ++y)
		for (uint32_t x = 0; x < tilemap.map_size.x; ++x)
			if (tilemap.entity_at(x, y) == PLAYER_ID)
			{
				player.position(vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)});
			}

	auto renderer = raycaster_t{{tilemap.map_size.x, tilemap.map_size.y}, fpg};
#if FOG_ENABLED
	renderer.fog_color = pal_find_closest(0, 0, 0);
	renderer.fog_start = real_t(0);
	renderer.fog_end = real_t(8);
#endif
	for (uint32_t x = 0; x < tilemap.map_size.x; ++x)
		for (uint32_t y = 0; y < tilemap.map_size.y; ++y)
		{
			renderer.tile({x, y}, tilemap.tile_at(x, y));
			renderer.floor({x, y}, tilemap.floor_at(x, y));
		}
	for (uint32_t y = 0; y < tilemap.map_size.y; ++y)
		for (uint32_t x = 0; x < tilemap.map_size.x; ++x)
		{
			const uint8_t id = tilemap.entity_at(x, y);
			if (id != 0 && id != PLAYER_ID)
				renderer.sprites.push_back({vec2_t{real_t(x + 0.5f), real_t(y + 0.5f)}, uint8_t(id - 1)});
		}

	while (screen_update(backbuffer))
	{
		player.update();
		renderer.render(player.cam, backbuffer, VIEWPORT);

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
