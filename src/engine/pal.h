#pragma once

#include <array>
#include "color.h"

using palcolor_t = uint8_t;

bool pal_load(const char *filename);
color_t pal_rgb(palcolor_t color_index);

// Returns the palette index whose RGB is closest (minimum squared distance) to (r, g, b) in 0-255 space.
// Call after pal_load().
palcolor_t pal_find_closest(uint8_t r, uint8_t g, uint8_t b);

// Sets the screen fade level per channel (0-200). 100,100,100 = normal; 0,0,0 = black; 200,200,200 = white.
// Updates the VGA palette instantly (affects the entire screen, zero per-pixel cost).
// Call any time after pal_load().
void pal_set_fade(int r_pct, int g_pct, int b_pct);
