#pragma once

#include <ctime>

struct game_state_t
{
	enum phase_t
	{
		PHASE_INTRO = 0,
		PHASE_PLAYING = 1,
		PHASE_FINISHING = 2,
		PHASE_WORKSHOP_MINIGAME = 3,
	};

	static constexpr int TARGET_FPS = 10;

	bool player_blocked = false;
	bool herbicide_held = false;
	bool vines_cleared = false;
	bool station_key_held = false;
	bool station_door_opened = false;
	bool tree_note_read = false;
	bool workshop_completed = false;

	clock_t loop_start_clock = 0;
	int saved_elapsed_ticks = 0;
	int num_loop = 1;

	int phase = PHASE_INTRO;

	// Minigame transition signals
	bool request_workshop_minigame = false;
	bool returning_from_workshop_minigame = false;
	bool workshop_minigame_won = false;

	int elapsed_ticks() const
	{
		return int((clock() - loop_start_clock) * TARGET_FPS / CLOCKS_PER_SEC);
	}
};
