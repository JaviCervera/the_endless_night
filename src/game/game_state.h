#pragma once

#include <ctime>

struct game_state_t
{
	enum phase_t
	{
		PHASE_INTRO = 0,
		PHASE_PLAYING = 1,
		PHASE_FINISHING = 2,
	};

	int adventure_state = 1;
	bool player_blocked = false;

	clock_t loop_start_clock = 0;
	int num_loop_in_state = 1;

	void advance_state()
	{
		adventure_state++;
		num_loop_in_state = 0;
	}

	int phase = PHASE_INTRO;
};
