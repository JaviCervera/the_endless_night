#pragma once

#include "../engine/screen.h"

struct input_t
{
	// We are only going to use -1, 0, 1
	int8_t forward = 0;
	int8_t turn = 0;
	bool action = false; // Edge-triggered: true only on the frame space is first pressed
	bool exit_minigame = false; // Edge-triggered: true only on the frame X is first pressed
	bool menu_left = false; // Edge-triggered: true only on the frame left is first pressed
	bool menu_right = false; // Edge-triggered: true only on the frame right is first pressed
};

input_t input_calculate()
{
	auto input = input_t{};
	if (screen_key(SCREEN_KEY_UP))
		input.forward = 1;
	else if (screen_key(SCREEN_KEY_DOWN))
		input.forward = -1;
	if (screen_key(SCREEN_KEY_RIGHT))
		input.turn = 1;
	else if (screen_key(SCREEN_KEY_LEFT))
		input.turn = -1;

	static bool prev_space = false;
	const bool space = screen_key(SCREEN_KEY_SPACE);
	input.action = space && !prev_space;
	prev_space = space;

	static bool prev_x = false;
	const bool x_key = screen_key(SCREEN_KEY_X);
	input.exit_minigame = x_key && !prev_x;
	prev_x = x_key;

	static bool prev_left = false;
	static bool prev_right = false;
	const bool left = screen_key(SCREEN_KEY_LEFT);
	const bool right = screen_key(SCREEN_KEY_RIGHT);
	input.menu_left = left && !prev_left;
	input.menu_right = right && !prev_right;
	prev_left = left;
	prev_right = right;

	return input;
}
