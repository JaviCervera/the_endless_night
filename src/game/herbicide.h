#pragma once

#include "actionable.h"
#include "entity_ids.h"

struct herbicide_t : public actionable_t
{
	herbicide_t(vec2_t pos) : actionable_t{HERBICIDE_NAME, pos, HERBICIDE_ID}
	{
		halved = true;
		collidable = false;
	}
};
