#include "raycaster.h"
#include "entity.h"

raycaster_t::raycaster_t(uvec2_t map_size, const fpg_t &fpg)
		: fpg(&fpg),
			map_size(map_size)
{
	const size_t count = static_cast<size_t>(map_size.x) * map_size.y;
	world_map.assign(count, 0);
	floor_map.assign(count, 0);
}

void raycaster_t::render(const camera_t &cam, pixmap_t &backbuffer, viewport_t vp, int detail)
{
	static_assert(TEX_SIZE == (1 << TEX_BITS), "TEX_BITS must equal log2(TEX_SIZE)");

	// Resolve zero-dimension viewport to full backbuffer.
	const int vp_x = vp.x;
	const int vp_y = vp.y;
	const int vp_w = (vp.w > 0) ? vp.w : backbuffer.stride();
	const int vp_h = (vp.h > 0) ? vp.h : int(backbuffer.size().y);
	const int vp_h2 = vp_h / 2;

	// Precomputed once per frame: convert divides by vp_w (constant) into multiplies.
	// inv_vp_w_2 is used in the camera_x calculation; inv_vp_w is used for the floor
	// row's ray-step computation (replaces a 64-bit idiv per floor row with two muls).
	const real_t inv_vp_w = real_t::from_raw(real_t::ONE / vp_w);
	const real_t inv_vp_w_2 = real_t::from_raw((2 * real_t::ONE) / vp_w);
	// Precomputed vp_h * ONE used in line_height and tile_screen_h — saves a 32-bit
	// mul per ray / per sprite.
	const int vp_h_one = vp_h * real_t::ONE;
	// For the floor row_dist we need vp_h2 * ONE, not vp_h * ONE — the row_dist
	// formula divides by (vp_h/2). Using vp_h_one here would double the world
	// distance and make the floor scroll at twice the speed of the walls.
	const int vp_h2_one = vp_h2 * real_t::ONE;

	const int map_w = static_cast<int>(map_size.x);
	const int map_h = static_cast<int>(map_size.y);

	const int bb_stride = backbuffer.stride();

	// Fill ceiling; when floor is disabled cover the full viewport, otherwise only the top half.
	{
		const int x_step = (detail == 2) ? 1 : 2;
		const int x_start = vp_x + ((detail == 2) ? 0 : detail);
#if RENDER_FLOOR
		const int y_end = vp_y + vp_h2;
#else
		const int y_end = vp_y + vp_h - 1;
#endif
		if (x_step == 1)
		{
			// Contiguous fill per row — std::fill_n lowers to a tight store loop.
			for (int y = vp_y; y <= y_end; ++y)
				std::fill_n(backbuffer.row(y) + x_start, vp_w, ceiling_color);
		}
		else
		{
			for (int y = vp_y; y <= y_end; ++y)
			{
				palcolor_t *row = backbuffer.row(y);
				for (int x = x_start; x < vp_x + vp_w; x += x_step)
					row[x] = ceiling_color;
			}
		}
	}

#if FOG_ENABLED
	// Precompute fog range inverse: maps [fog_start, fog_end) -> [0, 4) for Bayer dithering.
	const real_t fog_range = fog_end - fog_start;
	const real_t inv_fog_range = (fog_range > real_t(0)) ? (real_t(4) / fog_range) : real_t(0);
	// 2x2 Bayer matrix (values 0-3) for 4-level ordered dithering.
	static const int bayer[2][2] = {{0, 2}, {3, 1}};
#endif

#if RENDER_FLOOR
	const real_t ray_dir_x0 = cam.dir.x - cam.plane.x;
	const real_t ray_dir_y0 = cam.dir.y - cam.plane.y;
	const real_t ray_dir_x1 = cam.dir.x + cam.plane.x;
	const real_t ray_dir_y1 = cam.dir.y + cam.plane.y;
	// Hoisted out of the row loop: the row-direction delta divided by vp_w is
	// just a multiplication by the precomputed inverse.
	const real_t floor_dx = (ray_dir_x1 - ray_dir_x0) * inv_vp_w;
	const real_t floor_dy = (ray_dir_y1 - ray_dir_y0) * inv_vp_w;

	const int y_floor_base = vp_y + vp_h2 + 1;
	const int floor_step = (detail == 2) ? 1 : 2;
	const int y_floor_start = (detail == 2 || y_floor_base % 2 == detail) ? y_floor_base : y_floor_base + 1;
	for (int y = y_floor_start; y < vp_y + vp_h; y += floor_step)
	{
		const int row_y = y - vp_y - vp_h2;
		const real_t row_dist = real_t::from_raw(vp_h2_one / row_y);

		palcolor_t *bb_row = backbuffer.row(y);

#if FOG_ENABLED
		// Beyond fog_end: fill entire floor row with fog color, skip texture work.
		if (row_dist >= fog_end)
		{
			std::fill_n(bb_row + vp_x, vp_w, fog_color);
			continue;
		}

		int floor_fog_level = 0;
		int floor_bayer0, floor_bayer1; // bayer thresholds for even/odd columns on this row
		if (row_dist >= fog_start)
		{
			floor_fog_level = int((row_dist - fog_start) * inv_fog_range);
			if (floor_fog_level > 3)
				floor_fog_level = 3;
			floor_bayer0 = bayer[y & 1][0];
			floor_bayer1 = bayer[y & 1][1];
		}
#endif

		// Two real_t muls instead of two 64-bit divides per row.
		const real_t floor_step_x = row_dist * floor_dx;
		const real_t floor_step_y = row_dist * floor_dy;

		real_t floor_x = cam.pos.x + row_dist * ray_dir_x0;
		real_t floor_y = cam.pos.y + row_dist * ray_dir_y0;

		int prev_cell_x = INT_MIN;
		int prev_cell_y = INT_MIN;
		const pixmap_t *tex = nullptr;

		palcolor_t *bb_p = bb_row + vp_x;
		for (int x = vp_x; x < vp_x + vp_w; ++x, ++bb_p)
		{
			const int cell_x = static_cast<int>(real_floor(floor_x));
			const int cell_y = static_cast<int>(real_floor(floor_y));

			floor_x += floor_step_x;
			floor_y += floor_step_y;

#if FOG_ENABLED
			if (floor_fog_level > ((x & 1) ? floor_bayer1 : floor_bayer0))
			{
				*bb_p = fog_color;
				continue;
			}
#endif

			if (cell_x != prev_cell_x || cell_y != prev_cell_y)
			{
				prev_cell_x = cell_x;
				prev_cell_y = cell_y;
				int floor_type = 1;
				if (cell_x >= 0 && cell_x < map_w && cell_y >= 0 && cell_y < map_h)
					// Bounds already checked: use the unchecked indexer to skip the
					// duplicated bounds validation inside floor().
					floor_type = floor_unsafe(cell_x, cell_y);
				if (floor_type < 1)
					floor_type = 1;
				tex = fpg->map(static_cast<size_t>(floor_type - 1));
			}

			if (!tex)
				continue;

			// Texture is sampled at the post-increment floor_x/y, matching the
			// original code's intentional ordering. Don't reorder this without
			// also revisiting the cell change logic above.
			const int tex_x = (floor_x.raw & (real_t::ONE - 1)) >> (real_t::FRAC_BITS - TEX_BITS);
			const int tex_y = (floor_y.raw & (real_t::ONE - 1)) >> (real_t::FRAC_BITS - TEX_BITS);
			// Use the column cache directly (one indirection + one index) instead
			// of tex->pixel() which constructs a uvec2_t and re-does the multiply.
			*bb_p = tex->column(tex_x)[tex_y];
		}
	}
#endif

	// Wall casting: one vertical stripe per viewport column.
	z_buf.assign(static_cast<size_t>(vp_w), real_t::large());

	const int col_step = (detail == 2) ? 1 : 2;
	const int col_start = (detail == 2) ? 0 : detail;
	for (int col = col_start; col < vp_w; col += col_step)
	{
		// 32-bit mul instead of 32-bit idiv per column (vp_w is constant).
		const real_t camera_x = real_t::from_raw(col * inv_vp_w_2.raw - real_t::ONE);
		const real_t ray_dir_x = cam.dir.x + cam.plane.x * camera_x;
		const real_t ray_dir_y = cam.dir.y + cam.plane.y * camera_x;

		int map_x = int(cam.pos.x);
		int map_y = int(cam.pos.y);

		const real_t delta_dist_x = (ray_dir_x == real_t(0)) ? real_t::large() : real_abs(real_t(1) / ray_dir_x);
		const real_t delta_dist_y = (ray_dir_y == real_t(0)) ? real_t::large() : real_abs(real_t(1) / ray_dir_y);

		int step_x, step_y;
		real_t side_dist_x, side_dist_y;

		if (ray_dir_x < real_t(0))
		{
			step_x = -1;
			side_dist_x = (cam.pos.x - real_t(map_x)) * delta_dist_x;
		}
		else
		{
			step_x = 1;
			side_dist_x = (real_t(map_x) + real_t(1) - cam.pos.x) * delta_dist_x;
		}
		if (ray_dir_y < real_t(0))
		{
			step_y = -1;
			side_dist_y = (cam.pos.y - real_t(map_y)) * delta_dist_y;
		}
		else
		{
			step_y = 1;
			side_dist_y = (real_t(map_y) + real_t(1) - cam.pos.y) * delta_dist_y;
		}

		int side = 0;
		while (true)
		{
			if (side_dist_x < side_dist_y)
			{
				side_dist_x += delta_dist_x;
				map_x += step_x;
				side = 0;
			}
			else
			{
				side_dist_y += delta_dist_y;
				map_y += step_y;
				side = 1;
			}
			if (map_x < 0 || map_x >= map_w || map_y < 0 || map_y >= map_h)
				break;
			// Bounds already checked: skip the per-call bounds validation in tile().
			if (tile_unsafe(map_x, map_y) > 0)
				break;
		}

		if (map_x < 0 || map_x >= map_w || map_y < 0 || map_y >= map_h)
			continue;

		real_t perp_wall_dist = (side == 0)
																? side_dist_x - delta_dist_x
																: side_dist_y - delta_dist_y;
		if (perp_wall_dist < real_t(0.001f))
			perp_wall_dist = real_t(0.001f);
		z_buf[static_cast<size_t>(col)] = perp_wall_dist;

		// 32-bit divide: vp_h_one precomputed, perp_wall_dist.raw >= 66 after clamp.
		const int line_height = vp_h_one / perp_wall_dist.raw;
		const int line_h2 = line_height / 2;
		const int draw_start = vp_y + std::max(0, vp_h2 - line_h2);
		const int draw_end = vp_y + std::min(vp_h - 1, line_h2 + vp_h2);

		// Bounds already validated inside the DDA loop above.
		const int wall_type = tile_unsafe(map_x, map_y);
		const size_t tex_idx = static_cast<size_t>(wall_type - 1);
		const pixmap_t *tex = fpg->map(tex_idx);
		if (!tex)
			continue;

#if FOG_ENABLED
		// Beyond fog_end: fill with solid fog color, skip all texture work.
		if (perp_wall_dist >= fog_end)
		{
			palcolor_t *p = backbuffer.row(draw_start) + vp_x + col;
			for (int y = draw_start; y <= draw_end; ++y)
			{
				*p = fog_color;
				p += bb_stride;
			}
			continue;
		}

		// Between fog_start and fog_end: compute Bayer dither level (0-3).
		int fog_level = 0;
		if (perp_wall_dist >= fog_start)
		{
			fog_level = int((perp_wall_dist - fog_start) * inv_fog_range);
			if (fog_level > 3)
				fog_level = 3;
		}
		const int fog_col_bayer = bayer[0][col & 1]; // bayer row preselected for the column
		const int fog_col_bayer1 = bayer[1][col & 1];
#endif

		real_t wall_x = (side == 0)
												? cam.pos.y + perp_wall_dist * ray_dir_y
												: cam.pos.x + perp_wall_dist * ray_dir_x;
		// Keep only the fractional part (equivalent to wall_x -= real_floor(wall_x),
		// but avoids the subtraction and is one less register-shuffle on 486).
		wall_x.raw &= (real_t::ONE - 1);

		int tex_x = (wall_x.raw >> (real_t::FRAC_BITS - TEX_BITS)) & (TEX_SIZE - 1);
		if (side == 0 && ray_dir_x > real_t(0))
			tex_x = TEX_SIZE - tex_x - 1;
		if (side == 1 && ray_dir_y < real_t(0))
			tex_x = TEX_SIZE - tex_x - 1;

		// 32-bit divide: TEX_SIZE*ONE fits in int32
		const real_t tex_step = real_t::from_raw((TEX_SIZE * real_t::ONE) / line_height);
		real_t tex_pos = tex_step * (draw_start - vp_y - vp_h2 + line_h2);

		// Hoisted out of the inner y loop: side is constant per column, so the
		// WALL_DARK_SHIFT gate is a single per-column comparison.
#if WALL_DARK_SHIFT > 0
		const bool darken = (side == 1);
#endif

		const palcolor_t *tex_col = tex->column(tex_x);
		palcolor_t *p = backbuffer.row(draw_start) + vp_x + col;
		for (int y = draw_start; y <= draw_end; ++y, p += bb_stride)
		{
			// tex_pos is the texture row index in Q16.16; the integer part IS the row.
			const int tex_y = (tex_pos.raw >> real_t::FRAC_BITS) & (TEX_SIZE - 1);
			tex_pos += tex_step;
#if FOG_ENABLED
			// Bayer ordered dithering: select fog or texture based on 2x2 Bayer matrix.
			if (fog_level > ((y & 1) ? fog_col_bayer1 : fog_col_bayer))
			{
				*p = fog_color;
				continue;
			}
#endif
			palcolor_t color = tex_col[tex_y];
#if WALL_DARK_SHIFT > 0
			if (darken && color != 0)
			{
				int sub = color & 0xF;
				if (sub > WALL_DARK_SHIFT)
					sub = WALL_DARK_SHIFT;
				color -= sub;
			}
#endif
			*p = color;
		}
	}

	// Sprite casting: sort sprites back-to-front, project and draw with Z-buffer test.
	const size_t num_entities = entity_t::num_entities();
	ent_order.resize(num_entities);
	for (size_t i = 0; i < num_entities; ++i)
		ent_order[i] = i;
	std::sort(ent_order.begin(), ent_order.end(), [&](size_t a, size_t b)
						{
							const entity_t *ent_a = entity_t::get_entity(a);
							const entity_t *ent_b = entity_t::get_entity(b);
              const real_t da = (ent_a->pos.x - cam.pos.x) * (ent_a->pos.x - cam.pos.x) + (ent_a->pos.y - cam.pos.y) * (ent_a->pos.y - cam.pos.y);
              const real_t db = (ent_b->pos.x - cam.pos.x) * (ent_b->pos.x - cam.pos.x) + (ent_b->pos.y - cam.pos.y) * (ent_b->pos.y - cam.pos.y);
              return da > db; });

	const real_t inv_det = real_t(1) / (cam.plane.x * cam.dir.y - cam.dir.x * cam.plane.y);
	for (size_t idx = 0; idx < num_entities; ++idx)
	{
		const entity_t *ent = entity_t::get_entity(ent_order[idx]);

		if (!ent->active)
			continue;

		const real_t ent_x = ent->pos.x - cam.pos.x;
		const real_t ent_y = ent->pos.y - cam.pos.y;
		const real_t transform_x = inv_det * (cam.dir.y * ent_x - cam.dir.x * ent_y);
		const real_t transform_y = inv_det * (-cam.plane.y * ent_x + cam.plane.x * ent_y);

		// Near-plane cull: tiny positive transform_y blows up tile_screen_h and
		// screen_x (divide-by-near-zero), making the sprite cover the viewport and
		// pass the z-buffer test against every wall. Happens with sprites that are
		// almost perpendicular to the view direction (just barely in front).
		if (transform_y < real_t(0.05f))
			continue;

		const pixmap_t *tex = fpg->map(ent->fpg_id - 1);
		if (!tex)
			continue;

		const int tex_w = static_cast<int>(tex->size().x);
		const int tex_h = static_cast<int>(tex->size().y);

		const int screen_x = vp_x + static_cast<int>(real_t(vp_w / 2) * (real_t(1) + transform_x / transform_y));
		// Screen height for a 1-tile object at this distance
		// 32-bit divide: vp_h_one precomputed
		const int tile_screen_h = std::abs(vp_h_one / transform_y.raw);
		// Scale sprite proportionally to its texture size relative to TEX_SIZE
		int sp_h = tile_screen_h * tex_h / TEX_SIZE;
		int sp_w = tile_screen_h * tex_w / TEX_SIZE;
		if (ent->halved)
		{
			sp_h /= 2;
			sp_w /= 2;
		}
		if (sp_h == 0 || sp_w == 0)
			continue;

		// Sprite bottom is anchored to the floor at this distance; grows upward
		const int floor_screen_y = vp_y + vp_h2 + tile_screen_h / 2;
		const int draw_y0 = std::max(vp_y, floor_screen_y - sp_h);
		const int draw_y1 = std::min(vp_y + vp_h, floor_screen_y);
		const int draw_x0 = std::max(vp_x, screen_x - sp_w / 2);
		const int draw_x1 = std::min(vp_x + vp_w, screen_x + sp_w / 2);

		if (draw_x0 >= draw_x1 || draw_y0 >= draw_y1)
			continue;

		const int sp_left = screen_x - sp_w / 2;
		const real_t sp_tex_x_step = real_t::from_raw((tex_w * real_t::ONE) / sp_w);
		const real_t sp_tex_y_step = real_t::from_raw((tex_h * real_t::ONE) / sp_h);
		const real_t sp_tex_y_init = sp_tex_y_step * (draw_y0 - (floor_screen_y - sp_h));
		const int stripe_step = (detail == 2) ? 1 : 2;
		const int stripe_start = (detail == 2 || (draw_x0 - vp_x) % 2 == detail) ? draw_x0 : draw_x0 + 1;
		real_t sp_tex_x = sp_tex_x_step * (stripe_start - sp_left);
		const real_t sp_tex_x_inc = sp_tex_x_step * stripe_step;

#if FOG_ENABLED
		if (transform_y >= fog_end)
		{
			for (int stripe = stripe_start; stripe < draw_x1; stripe += stripe_step)
				if (transform_y < z_buf[static_cast<size_t>(stripe - vp_x)])
				{
					// Vertical column: stride-based pointer walk, not std::fill_n
					// (which would run horizontally in row-major storage).
					palcolor_t *p = backbuffer.row(draw_y0) + stripe;
					for (int y = draw_y0; y < draw_y1; ++y)
					{
						*p = fog_color;
						p += bb_stride;
					}
				}
			continue;
		}

		int sp_fog_level = 0;
		if (transform_y >= fog_start)
		{
			sp_fog_level = int((transform_y - fog_start) * inv_fog_range);
			if (sp_fog_level > 3)
				sp_fog_level = 3;
		}
#endif

		for (int stripe = stripe_start; stripe < draw_x1; stripe += stripe_step, sp_tex_x += sp_tex_x_inc)
		{
			if (transform_y >= z_buf[static_cast<size_t>(stripe - vp_x)])
				continue;

			const int tex_x = int(sp_tex_x);
			const palcolor_t *col = tex->column(tex_x);
			real_t sp_tex_y = sp_tex_y_init;
#if FOG_ENABLED
			const int sp_bayer0 = bayer[0][stripe & 1];
			const int sp_bayer1 = bayer[1][stripe & 1];
#endif

			palcolor_t *p = backbuffer.row(draw_y0) + stripe;
			for (int y = draw_y0; y < draw_y1; ++y, sp_tex_y += sp_tex_y_step, p += bb_stride)
			{
				const int tex_y = int(sp_tex_y);
				const palcolor_t color = col[tex_y];
				if (color != 0)
				{
#if FOG_ENABLED
					if (sp_fog_level > ((y & 1) ? sp_bayer1 : sp_bayer0))
						*p = fog_color;
					else
#endif
						*p = color;
				}
			}
		}
	}
}
