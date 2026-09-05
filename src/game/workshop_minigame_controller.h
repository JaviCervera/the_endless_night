#pragma once

#include "controller.h"
#include "game_state.h"
#include "../engine/pixmap.h"
#include "../engine/pal.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

struct workshop_minigame_controller_t : public controller_t
{
	enum state_t { FADE_IN, RUNNING, FADE_OUT };

	workshop_minigame_controller_t(game_state_t *game, pixmap_t *backbuffer)
			: game{game}, backbuffer{backbuffer}, state{FADE_IN}
	{
	}

	game_state_t *game;
	pixmap_t *backbuffer;
	state_t state;

	// Minigame data
	uint8_t solution[3];              // 3 unique correct pieces (0-7)
	uint8_t columns[8][3];            // Up to 8 columns × 3 pieces
	uint8_t column_heights[8] = {0};  // Fill level per column (0-3)
	uint8_t column_scores[8] = {0};   // Score per column (0-3)
	uint8_t num_columns = 0;          // Total columns filled (0-8)
	uint8_t current_column = 0;       // Active column (0-7)
	uint8_t cursor_pos = 0;           // Bottom row cursor (0-7)
	uint32_t timer_ticks = 600;       // 60 sec × 10 FPS
	bool game_won = false;
	bool minigame_started = false;
	bool piece_available[8] = {true, true, true, true, true, true, true, true};

	static constexpr const char* piece_names[8] = {"A","B","C","D","E","F","G","H"};

	void reset()
	{
		// Re-seed random for new solution
		srand(static_cast<unsigned>(clock()));

		uint8_t pool[8] = {0,1,2,3,4,5,6,7};
		for (int i = 7; i > 0; --i)
		{
			int j = rand() % (i + 1);
			std::swap(pool[i], pool[j]);
		}
		solution[0] = pool[0];
		solution[1] = pool[1];
		solution[2] = pool[2];

		std::memset(columns, 0, sizeof(columns));
		std::memset(column_heights, 0, sizeof(column_heights));
		std::memset(column_scores, 0, sizeof(column_scores));
		for (int i = 0; i < 8; ++i) piece_available[i] = true;
		num_columns = 0;
		current_column = 0;
		cursor_pos = 0;
		timer_ticks = 600;
		game_won = false;
		minigame_started = false;
		state = FADE_IN;
	}

	void update(const input_t &input) override
	{
		if (state == FADE_IN)
		{
			if (!pal_fade_active())
				pal_start_fade(100, 100, 100, 4);
			
			if (!pal_update_fade())
				state = RUNNING;
		}
		else if (state == FADE_OUT)
		{
			if (!pal_update_fade())
			{
				game->loop_start_clock = clock() - (clock_t)(game->saved_elapsed_ticks * CLOCKS_PER_SEC / 10);
				game->returning_from_workshop_minigame = true;
				game->phase = game_state_t::PHASE_PLAYING;
				state = FADE_IN;
			}
			// Continue rendering during fade-out
		}
		else // RUNNING
		{
			if (!minigame_started)
			{
				reset();
				minigame_started = true;
			}

			if (timer_ticks > 0) timer_ticks--;
			if (timer_ticks == 0)
			{
				pal_start_fade(0, 0, 0, 4);
				state = FADE_OUT;
			}

			if (input.menu_left)
			{
				int next = cursor_pos;
				do { next = (next + 7) % 8; } while (!piece_available[next]);
				cursor_pos = next;
			}
			if (input.menu_right)
			{
				int next = cursor_pos;
				do { next = (next + 1) % 8; } while (!piece_available[next]);
				cursor_pos = next;
			}
			if (input.action)
				place_piece();

			if (game_won)
			{
				pal_start_fade(0, 0, 0, 4);
				state = FADE_OUT;
			}
		}

		render_minigame();
	}

private:
	void init_minigame()
	{
		srand(static_cast<unsigned>(clock()));

		uint8_t pool[8] = {0,1,2,3,4,5,6,7};
		for (int i = 7; i > 0; --i)
		{
			int j = rand() % (i + 1);
			std::swap(pool[i], pool[j]);
		}
		solution[0] = pool[0];
		solution[1] = pool[1];
		solution[2] = pool[2];

		std::memset(columns, 0, sizeof(columns));
		std::memset(column_heights, 0, sizeof(column_heights));
		std::memset(column_scores, 0, sizeof(column_scores));
		for (int i = 0; i < 8; ++i) piece_available[i] = true;
		num_columns = 0;
		current_column = 0;
		cursor_pos = 0;
		timer_ticks = 600;
		game_won = false;
	}

void place_piece()
	{
		if (column_heights[current_column] >= 3) return;
		if (!piece_available[cursor_pos]) return;

		columns[current_column][column_heights[current_column]++] = cursor_pos;
		piece_available[cursor_pos] = false;

		if (column_heights[current_column] == 3)
		{
			column_scores[current_column] = count_correct(current_column);

			if (column_scores[current_column] == 3)
			{
				game_won = true;
				return;
			}

			current_column++;
			if (current_column >= 8)
			{
				// All 8 columns filled without win - clear everything and restart
				std::memset(columns, 0, sizeof(columns));
				std::memset(column_heights, 0, sizeof(column_heights));
				std::memset(column_scores, 0, sizeof(column_scores));
				num_columns = 0;
				current_column = 0;
				// Restore all pieces (they were marked unavailable when placed in 8th column)
				for (int i = 0; i < 8; ++i) piece_available[i] = true;
			}
			else
			{
				if (num_columns < 8)
					num_columns++;
				// Restore all pieces for next column
				for (int i = 0; i < 8; ++i) piece_available[i] = true;
			}
		}
		else
		{
			// 1st or 2nd piece: move cursor to next available
			int next = cursor_pos;
			do { next = (next + 1) % 8; } while (!piece_available[next]);
			cursor_pos = next;
		}
	}

	int count_correct(int col)
	{
		int correct = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (columns[col][i] == solution[j])
				{
					correct++;
					break;
				}
			}
		}
		return correct;
	}

	void render_minigame()
	{
		backbuffer->fill(0);

		// Timer at top center
		int minutes = timer_ticks / 600;
		int seconds = (timer_ticks % 600) / 10;
		char timer_str[16];
		std::snprintf(timer_str, sizeof(timer_str), "%02d:%02d", minutes, seconds);
		int timer_x = (320 - text_length(font, timer_str)) / 2;
		backbuffer->text(timer_str, {static_cast<uint32_t>(timer_x), 4}, 15);

		// Columns: up to 8 columns, each showing pieces as they are placed + score when complete
		const int col_x_start = 40;
		const int col_x_spacing = 30;
		const int col_y_start = 20;

		// Render up to current_column (inclusive), max 8
		int max_col = current_column < 8 ? current_column + 1 : 8;
		for (int c = 0; c < max_col; ++c)
		{
			int x = col_x_start + c * col_x_spacing;
			// Completed columns show all 3 pieces; active column shows pieces placed so far
			int max_row = (c < num_columns) ? 3 : column_heights[c];
			
			for (int r = 0; r < max_row; ++r)
			{
				uint8_t piece = columns[c][r];
				backbuffer->text(piece_names[piece],
					{static_cast<uint32_t>(x), static_cast<uint32_t>(col_y_start + r * 10)}, 15);
			}
			
			// Score ONLY for completed columns (below the 3 pieces)
			if (c < num_columns)
			{
				char score_str[4];
				std::snprintf(score_str, sizeof(score_str), "%d", column_scores[c]);
				backbuffer->text(score_str,
					{static_cast<uint32_t>(x), static_cast<uint32_t>(col_y_start + 35)}, 15);
			}
		}

		// Bottom row: 8 pieces A-H (only available ones)
		const int bottom_y = 160;
		const int bottom_x_start = 40;
		for (int i = 0; i < 8; ++i)
		{
			if (piece_available[i])
			{
				backbuffer->text(piece_names[i],
					{static_cast<uint32_t>(bottom_x_start + i * 20), static_cast<uint32_t>(bottom_y)}, 15);
			}
		}
		// Cursor ^ under selected piece (yellow)
		backbuffer->text("^",
			{static_cast<uint32_t>(bottom_x_start + cursor_pos * 20), static_cast<uint32_t>(bottom_y + 10)}, 14);
	}
};