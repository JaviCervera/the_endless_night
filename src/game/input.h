#pragma once

#include "../engine/screen.h"

struct input_t
{
	// We are only going to use -1, 0, 1
	int8_t forward = 0;
	int8_t turn = 0;
	bool action = false; // Edge-triggered: true only on the frame space is first pressed
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

	return input;
}
