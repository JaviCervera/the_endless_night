#pragma once

#include "actionable.h"
#include "entity_ids.h"

struct radio_station_t : public actionable_t
{
	radio_station_t(vec2_t pos) : actionable_t{RADIO_STATION_NAME, pos, RADIO_STATION_ID}
	{
		collidable = true;
		action_text = "Take power generator";
	}

	void on_action_pressed() override
	{
		if (!already_picked)
		{
			dialog_lines.push_back("I got a power generator!");
			already_picked = true;
		}
		else
		{
			dialog_lines.push_back("I already have the power generator");
		}
	}

private:
	inline static auto already_picked = false;
};
