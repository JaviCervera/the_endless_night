#pragma once

#include "input.h"

struct controller_t
{
	virtual ~controller_t() = default;
	virtual void update(const input_t &input) = 0;
	virtual void stop() {}
};
