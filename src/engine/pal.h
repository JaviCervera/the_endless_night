#pragma once

#include <array>
#include "color.h"

using palcolor_t = uint8_t;

bool pal_load(const char *filename);
color_t pal_rgb(palcolor_t color_index);

palcolor_t pal_find_closest(uint8_t r, uint8_t g, uint8_t b);

void pal_set_fade(int r_pct, int g_pct, int b_pct);

void pal_start_fade(int target_r, int target_g, int target_b, int steps);
bool pal_update_fade();
bool pal_fade_active();
