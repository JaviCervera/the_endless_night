#pragma once

#include <cmath>
#include <cstdint>
#include "real.h"

template <typename T>
struct vec2_base_t
{
	T x, y;

	vec2_base_t(T x = T(), T y = T())
			: x(x), y(y) {}

	bool operator==(const vec2_base_t &other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const vec2_base_t &other) const
	{
		return !(*this == other);
	}

	vec2_base_t operator+(const vec2_base_t &other) const
	{
		return vec2_base_t(x + other.x, y + other.y);
	}

	vec2_base_t operator+(T scalar) const
	{
		return vec2_base_t(x + scalar, y + scalar);
	}

	vec2_base_t &operator+=(const vec2_base_t &other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	vec2_base_t &operator+=(T scalar)
	{
		x += scalar;
		y += scalar;
		return *this;
	}

	vec2_base_t operator-(const vec2_base_t &other) const
	{
		return vec2_base_t(x - other.x, y - other.y);
	}

	vec2_base_t operator-(T scalar) const
	{
		return vec2_base_t(x - scalar, y - scalar);
	}

	vec2_base_t operator-() const
	{
		return vec2_base_t(-x, -y);
	}

	vec2_base_t &operator-=(const vec2_base_t &other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	vec2_base_t &operator-=(T scalar)
	{
		x -= scalar;
		y -= scalar;
		return *this;
	}

	vec2_base_t operator*(const vec2_base_t &other) const
	{
		return vec2_base_t(x * other.x, y * other.y);
	}

	vec2_base_t operator*(T scalar) const
	{
		return vec2_base_t(x * scalar, y * scalar);
	}

	vec2_base_t &operator*=(const vec2_base_t &other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	vec2_base_t &operator*=(T scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	vec2_base_t operator/(const vec2_base_t &other) const
	{
		return vec2_base_t(x / other.x, y / other.y);
	}

	vec2_base_t operator/(T scalar) const
	{
		return vec2_base_t(x / scalar, y / scalar);
	}

	vec2_base_t &operator/=(const vec2_base_t &other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	vec2_base_t &operator/=(T scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	const T *data() const
	{
		return &x;
	}

	T *data()
	{
		return &x;
	}

	T dot(const vec2_base_t &other) const
	{
		return x * other.x + y * other.y;
	}

	T sqlength() const
	{
		return dot(*this);
	}

	T length() const
	{
		return T(std::sqrt(float(sqlength())));
	}

	vec2_base_t norm() const
	{
		const T len = length();
		const T invLen = (len > T()) ? (T(1) / len) : T();
		return *this * invLen;
	}

	vec2_base_t mix(const vec2_base_t &other, T t) const
	{
		return *this + (other - *this) * t;
	}
};

using ivec2_t = vec2_base_t<int32_t>;
using uvec2_t = vec2_base_t<uint32_t>;
using vec2_t = vec2_base_t<real_t>;
