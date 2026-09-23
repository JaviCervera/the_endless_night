#pragma once

#include <string>
#include "input.h"
#include "../engine/pixmap.h"
#include "../engine/viewport.h"

struct menu_t
{
	static constexpr int OPTION_COUNT = 2;
	static constexpr int MARKER_SIZE = 8;
	static constexpr int MARKER_GAP = 8;
	static constexpr int ROW_HEIGHT = 12;
	static constexpr palcolor_t COLOR_SELECTED_TEXT = 15;
	static constexpr palcolor_t COLOR_UNSELECTED = 8;
	static constexpr palcolor_t COLOR_SELECTED_MARKER = 22;

	menu_t(std::string option0, std::string option1)
			: options{option0, option1} {}

	void reset()
	{
		selection = 0;
	}

	void set_options(const std::string &option0, const std::string &option1)
	{
		options[0] = option0;
		options[1] = option1;
	}

	bool update(const input_t &input)
	{
		if (input.menu_up)
			selection = 0;
		if (input.menu_down)
			selection = OPTION_COUNT - 1;
		return input.accept;
	}

	int selected() const
	{
		return selection;
	}

	static uvec2_t lower_left(viewport_t viewport)
	{
		return {uint32_t(viewport.x + 8), uint32_t(viewport.y + viewport.h - 32)};
	}

	void draw(pixmap_t &backbuffer, uvec2_t pos) const
	{
		for (int i = 0; i < OPTION_COUNT; ++i)
		{
			const bool is_selected = (i == selection);
			const auto text_color = is_selected ? COLOR_SELECTED_TEXT : COLOR_UNSELECTED;
			const auto marker_color = is_selected ? COLOR_SELECTED_MARKER : COLOR_UNSELECTED;
			const uint32_t y = pos.y + uint32_t(i * ROW_HEIGHT);
			backbuffer.rectfill({pos.x, y}, {MARKER_SIZE, MARKER_SIZE}, marker_color);
			backbuffer.text(options[i].c_str(), {pos.x + MARKER_SIZE + MARKER_GAP, y}, text_color);
		}
	}

private:
	std::string options[OPTION_COUNT];
	int selection = 0;
};
