#include <algorithm>
#include <ctime>
#include <allegro.h>
#include "pixmap.h"
#include "real.h"
#include "screen.h"
#include "vec2.h"

static auto _frame_time = 1000 / 60;
static auto _frame_ticks = clock_t{};
static auto _target_fps = uint16_t{60};
static auto _prev_time = clock_t{};
static auto _fps_time = clock_t{};
static auto _frame_count = uint16_t{0};
static auto _current_fps = uint16_t{0};

void screen_open(const char *title, uvec2_t size, uint16_t target_fps)
{
	allegro_init();
	install_keyboard();
	install_timer();
	set_color_depth(8);
#ifdef ALLEGRO_DJGPP
	set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, size.x, size.y, 0, 0);
#else
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, size.x, size.y, 0, 0);
#endif
	set_window_title(title);
	_frame_time = 1000 / target_fps;
	_frame_ticks = (clock_t)(_frame_time * CLOCKS_PER_SEC / 1000);
	_target_fps = target_fps;
	_prev_time = clock();
	_fps_time = _prev_time;
	_frame_count = 0;
	_current_fps = 0;
}

bool screen_update(const pixmap_t &pixmap)
{
	if (key[KEY_ESC])
		return false;

	auto *bitmap = *reinterpret_cast<BITMAP **>(const_cast<pixmap_t *>(&pixmap));
	blit(bitmap, screen, 0, 0, 0, 0, bitmap->w, bitmap->h);

	// Spin-wait instead of rest() to avoid interfering with
	// Allegro's timer-driven sound mixing (rest() yields the CPU
	// and can cause audio buffer underruns on DOS).
	while (static_cast<int>((clock() - _prev_time) * 1000 / CLOCKS_PER_SEC) < _frame_time)
		;

	const auto post_wait = clock();
	// Use ideal target time so small overshoots self-correct next frame,
	// but clamp to never fall more than one frame behind actual time.
	const auto target_time = _prev_time + _frame_ticks;
	_prev_time = std::max(target_time, post_wait - _frame_ticks);

	// Count FPS after the sleep so we measure completed frames.
	++_frame_count;
	if ((post_wait - _fps_time) * 1000 / CLOCKS_PER_SEC >= 1000)
	{
		_current_fps = _frame_count;
		_frame_count = 0;
		_fps_time = post_wait;
	}

	return true;
}

void screen_close()
{
	allegro_exit();
}

bool screen_key(int key_code)
{
	switch (key_code)
	{
	case SCREEN_KEY_UP:
		return key[KEY_UP] != 0;
	case SCREEN_KEY_DOWN:
		return key[KEY_DOWN] != 0;
	case SCREEN_KEY_LEFT:
		return key[KEY_LEFT] != 0;
	case SCREEN_KEY_RIGHT:
		return key[KEY_RIGHT] != 0;
	case SCREEN_KEY_SPACE:
		return key[KEY_SPACE] != 0;
	case SCREEN_KEY_X:
		return key[KEY_X] != 0;
	default:
		return false;
	}
}

uint16_t screen_target_fps()
{
	return _target_fps;
}

uint16_t screen_current_fps()
{
	return _current_fps;
}
