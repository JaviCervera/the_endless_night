#pragma once

#include "actor.h"

struct farmer_t : public actor_t
{
	farmer_t()
	{
		action_text = "Talk to farmer";
	}
};
