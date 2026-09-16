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
		PHASE_TOWER_MINIGAME = 4,
	};

	static constexpr int TARGET_FPS = 10;

	enum carried_generator_t { CARRIED_NONE = -1, CARRIED_BARN_GENERATOR = 0, CARRIED_RADIO_GENERATOR = 1, CARRIED_WORKSHOP_GENERATOR = 2 };
	carried_generator_t carried_generator = CARRIED_NONE;

	bool generator_placed[3] = {false, false, false};
	int tower_generator[4] = {-1, -1, -1, -1};
	int num_generators_placed = 0;

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
	bool request_tower_minigame = false;
	bool returning_from_tower_minigame = false;

	void advance_loop()
	{
		num_loop++;

		herbicide_held = false;
		station_key_held = false;
		carried_generator = CARRIED_NONE;
		if (!generator_placed[CARRIED_WORKSHOP_GENERATOR])
			workshop_completed = false;

		phase = PHASE_INTRO;
	}

	int elapsed_ticks() const
	{
		return int((clock() - loop_start_clock) * TARGET_FPS / CLOCKS_PER_SEC);
	}
};
