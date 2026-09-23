#pragma once

#include "actionable.h"
#include "entity_ids.h"

struct plant_t : public actionable_t
{
	plant_t(texts_t *t, vec2_t pos) : actionable_t{PLANT_NAME, pos, PLANT_ID, t}
	{
		halved = true;
		collidable = false;
	}
};
