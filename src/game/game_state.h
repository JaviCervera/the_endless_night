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

	bool player_blocked = false;
	bool herbicide_held = false;
	bool vines_cleared = false;
	bool station_key_held = false;
	bool station_door_opened = false;
	bool tree_note_read = false;

	clock_t loop_start_clock = 0;
	int num_loop = 1;

	int phase = PHASE_INTRO;
};
