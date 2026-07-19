#pragma once

#include <ctime>
#include "../engine/vec2.h"

struct crowbar_t;
struct generator_t;

struct game_state_t
{
	enum phase_t
	{
		PHASE_INTRO = 0,
		PHASE_PLAYING = 1,
		PHASE_FINISHING = 2,
	};

	int adventure_state = 1;
	bool generator_picked = false;
	bool player_blocked = false;

	vec2_t player_end_pos;

	clock_t loop_start_clock = 0;
	int num_loop_in_state = 1;
	int last_loop_state = 1;

	void advance_state()
	{
		adventure_state++;
		num_loop_in_state = 0;
	}

	int phase = PHASE_INTRO;
	int intro_timer = 0;
	int intro_sub = 0;

	int finish_ticks = 0;

	bool generator_alive = false;
};
