#pragma once

#include "actionable.h"
#include "entity_ids.h"

struct station_key_t : public actionable_t
{
	station_key_t(vec2_t pos) : actionable_t{KEY_NAME, pos, KEY_ID}
	{
		halved = true;
		collidable = false;
	}
};
