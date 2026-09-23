#pragma once

#include "actionable.h"
#include "entity_ids.h"
#include "game_state.h"

struct fence_t : public actionable_t
{
	fence_t(texts_t *t, game_state_t *game, vec2_t pos) : actionable_t{FENCE_NAME, pos, FENCE_ID, t}
	{
		if (game->num_generators_placed >= 2)
		{
			active = false;
			collidable = false;
		}
	}
};
