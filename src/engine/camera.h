#pragma once

#include "vec2.h"

struct camera_t
{
	vec2_t pos;		// Player position
	vec2_t dir;		// Direction vector (unit length)
	vec2_t plane; // Camera plane (perpendicular to direction)

	camera_t(
			const vec2_t &pos = {},
			const vec2_t &dir = vec2_t{real_t(1), real_t(0)},
			const vec2_t &plane = vec2_t{real_t(0), real_t(0.66f)})
			: pos(pos), dir(dir), plane(plane) {}
};
