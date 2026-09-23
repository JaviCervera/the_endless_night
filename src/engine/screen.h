#pragma once

#include <cstdint>
#include "pixmap.h"
#include "vec2.h"

#define SCREEN_KEY_UP 0
#define SCREEN_KEY_DOWN 1
#define SCREEN_KEY_LEFT 2
#define SCREEN_KEY_RIGHT 3
#define SCREEN_KEY_SPACE 4
#define SCREEN_KEY_X 5
#define SCREEN_KEY_ENTER 6
#define SCREEN_KEY_ESC 7

void screen_open(const char *title, uvec2_t size, uint16_t target_fps);
bool screen_load_font(const char *filename);
bool screen_update(const pixmap_t &pixmap);
void screen_close();
bool screen_key(int key_code);
uint16_t screen_target_fps();
uint16_t screen_current_fps();
