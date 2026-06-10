#include "raycaster.h"

raycaster_t::raycaster_t(uvec2_t map_size, const fpg_t &fpg)
		: fpg(&fpg),
			map_size(map_size)
{
	const size_t count = static_cast<size_t>(map_size.x) * map_size.y;
	world_map.assign(count, 0);
	floor_map.assign(count, 0);
}
