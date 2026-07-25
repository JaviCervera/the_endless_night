#pragma once

#include "actionable.h"
#include "entity_ids.h"

struct plant_t : public actionable_t
{
	plant_t(vec2_t pos) : actionable_t{PLANT_NAME, pos, PLANT_ID}
	{
		halved = true;
		collidable = false;
	}
};
