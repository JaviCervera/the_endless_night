#pragma once

#include "actionable.h"
#include "entity_ids.h"

struct power_tower_t : public actionable_t
{
	power_tower_t(vec2_t pos) : actionable_t{POWER_TOWER_NAME, pos, POWER_TOWER_ID}
	{
		collidable = true;
		action_text = "Connect power generator";
	}

	void on_action_pressed() override
	{
	}
};
