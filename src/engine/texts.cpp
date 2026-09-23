#include "texts.h"

#include <fstream>

namespace
{
	std::string trim(const std::string &text)
	{
		const auto first = text.find_first_not_of(" \t\r\n");
		if (first == std::string::npos)
			return {};
		const auto last = text.find_last_not_of(" \t\r\n");
		return text.substr(first, last - first + 1);
	}
}

texts_t::texts_t(const char *filename)
{
	std::ifstream file{filename};
	std::string line;
	bool first_line = true;
	while (std::getline(file, line))
	{
		if (first_line)
		{
			// Tolerate a UTF-8 byte order mark added by an editor.
			if (line.size() >= 3u &&
					static_cast<unsigned char>(line[0]) == 0xEF &&
					static_cast<unsigned char>(line[1]) == 0xBB &&
					static_cast<unsigned char>(line[2]) == 0xBF)
				line.erase(0, 3);
			first_line = false;
		}

		line = trim(line);
		if (line.empty() || line[0] == ';' || line[0] == '#')
			continue;

		const auto separator = line.find('=');
		if (separator == std::string::npos)
			continue;

		const auto key = trim(line.substr(0, separator));
		if (key.empty())
			continue;

		m_texts[key] = trim(line.substr(separator + 1));
	}
}

std::string texts_t::get(const char *key) const
{
	const auto it = m_texts.find(key);
	if (it == m_texts.end())
		return key;
	return it->second;
}
