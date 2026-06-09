#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <allegro.h>
#include "../engine/pixmap.h"

struct banner_t
{
	static constexpr int LINE_HEIGHT = 8;
	static constexpr int MAX_LINE_WIDTH = 300;
	static constexpr int LINES_PER_PAGE = 2;
	static constexpr int TICKS_PER_PAGE = 5 * 12; // 5 seconds * desired FPS

	void show(const std::string &text)
	{
		m_lines.clear();
		m_current_line = 0;
		m_timer = 0;

		std::string line;
		std::string word;
		size_t pos = 0;

		while (pos < text.length())
		{
			while (pos < text.length() && text[pos] == ' ')
				pos++;
			if (pos >= text.length())
				break;

			size_t word_start = pos;
			while (pos < text.length() && text[pos] != ' ')
				pos++;
			word = text.substr(word_start, pos - word_start);

			std::string test = line.empty() ? word : line + " " + word;
			if (text_length(font, test.c_str()) > MAX_LINE_WIDTH && !line.empty())
			{
				m_lines.push_back(line);
				line = word;
			}
			else
			{
				line = test;
			}
		}
		if (!line.empty())
			m_lines.push_back(line);

		if (!m_lines.empty())
			m_timer = TICKS_PER_PAGE;
	}

	void update()
	{
		if (!active())
			return;

		m_timer--;
		if (m_timer <= 0)
		{
			m_current_line += LINES_PER_PAGE;
			if (m_current_line < static_cast<int>(m_lines.size()))
				m_timer = TICKS_PER_PAGE;
		}
	}

	void draw(pixmap_t &backbuffer) const
	{
		if (!active())
			return;

		int end = std::min(m_current_line + LINES_PER_PAGE, static_cast<int>(m_lines.size()));
		for (int i = m_current_line; i < end; i++)
		{
			uint32_t y = static_cast<uint32_t>(m_pos.y + (i - m_current_line) * LINE_HEIGHT);
			backbuffer.text(m_lines[i].c_str(), {m_pos.x, y}, 15);
		}
	}

	bool active() const
	{
		return m_current_line < static_cast<int>(m_lines.size());
	}

	void clear()
	{
		m_lines.clear();
		m_current_line = 0;
		m_timer = 0;
	}

private:
	std::vector<std::string> m_lines;
	int m_current_line = 0;
	int m_timer = 0;
	uvec2_t m_pos{10, 2};
};
