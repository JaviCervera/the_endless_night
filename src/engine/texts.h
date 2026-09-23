#pragma once

#include <cstdio>
#include <map>
#include <string>

// Localized game texts. Constructed from a key=value .ini file (see
// assets/lang/en.ini). Unknown keys return the key itself so missing
// translations are obvious in-game.
struct texts_t
{
	explicit texts_t(const char *filename);

	std::string get(const char *key) const;

	// printf-style formatting on a localized format string, e.g.
	// format("loop_banner", 3) with "loop_banner=Loop %d".
	template <typename... Args>
	std::string format(const char *key, Args... args) const
	{
		char buffer[256];
		std::snprintf(buffer, sizeof(buffer), get(key).c_str(), args...);
		return buffer;
	}

	bool loaded() const
	{
		return !m_texts.empty();
	}

private:
	std::map<std::string, std::string> m_texts;
};
