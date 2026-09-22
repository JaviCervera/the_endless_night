#pragma once

#include <climits>
#include <cmath>
#include <cstdint>

// Q16.16 fixed-point arithmetic. The integer part occupies the upper 16 bits
// and the fractional part the lower 16 bits (scale factor: 65536).
// All arithmetic uses integer instructions — no FPU required.
// The float and int constructors are constexpr so literals like real_t(0.66f)
// and real_t(3) are resolved at compile time with zero runtime FPU cost.
struct real_t
{
	static constexpr int32_t FRAC_BITS = 16;
	static constexpr int32_t ONE = 1 << FRAC_BITS;

	int32_t raw;

	constexpr real_t() noexcept : raw(0) {}
	explicit constexpr real_t(float f) noexcept : raw(static_cast<int32_t>(f * ONE)) {}
	explicit constexpr real_t(int i) noexcept : raw(static_cast<int32_t>(i) << FRAC_BITS) {}

	static constexpr real_t from_raw(int32_t r) noexcept
	{
		real_t v;
		v.raw = r;
		return v;
	}

	// Sentinel: value larger than any possible map diagonal. Used as DDA "infinity".
	static constexpr real_t large() noexcept { return from_raw(0x7FFFFFFF); }

	explicit constexpr operator float() const noexcept { return static_cast<float>(raw) / static_cast<float>(ONE); }
	explicit constexpr operator int() const noexcept { return raw >> FRAC_BITS; }
	explicit constexpr operator uint32_t() const noexcept { return static_cast<uint32_t>(raw >> FRAC_BITS); }

	constexpr real_t operator+(real_t o) const noexcept { return from_raw(raw + o.raw); }
	constexpr real_t operator-(real_t o) const noexcept { return from_raw(raw - o.raw); }
	constexpr real_t operator-() const noexcept { return from_raw(-raw); }

	constexpr real_t operator*(real_t o) const noexcept
	{
		return from_raw(static_cast<int32_t>((static_cast<int64_t>(raw) * o.raw) >> FRAC_BITS));
	}

	real_t operator/(real_t other) const noexcept
	{
		if (other.raw == 0)
			return (raw >= 0) ? large() : from_raw(-large().raw);
		/*
		#ifdef __DJGPP__
				// Guard against idiv overflow: result must fit in Q16.16 range (~[-32768, 32768)).
				// Overflow when abs(raw) / abs(other.raw) >= 2^(FRAC_BITS-1) = 32768.
				// Equivalent fast check: abs(raw) >> (FRAC_BITS-1) >= abs(other.raw).
				const uint32_t abs_a = (raw >= 0) ? (uint32_t)raw : (uint32_t)(-raw);
				const uint32_t abs_b = (other.raw >= 0) ? (uint32_t)other.raw : (uint32_t)(-other.raw);
				if ((abs_a >> (FRAC_BITS - 1)) >= abs_b)
				{
					// Same sign → positive infinity, different sign → negative infinity.
					return ((raw ^ other.raw) >= 0) ? large() : from_raw(-large().raw);
				}
				// Use 486 native 64/32 idivl: EDX:EAX (= raw << FRAC_BITS) ÷ other.raw → EAX.
				// Avoids __divdi3 entirely.
				int32_t result;
				__asm__(
						"idivl %3"
						: "=a"(result)
						: "d"(raw >> FRAC_BITS), "a"((uint32_t)raw << FRAC_BITS), "rm"(other.raw)
						: "cc");
				return from_raw(result);
		#else
		*/
		const int64_t result64 = (int64_t(raw) << FRAC_BITS) / int64_t(other.raw);
		if (result64 > INT32_MAX)
			return large();
		if (result64 < INT32_MIN)
			return from_raw(-large().raw);
		return from_raw(int32_t(result64));
		// #endif
	}

	// Fast Q16.16 reciprocal (1 / *this) for hot paths. On DJGPP/386+ this uses
	// one 32-bit idivl with a 64-bit dividend (EDX:EAX = 2^32) instead of the
	// __divdi3 libcall that operator/ emits. Saturates to +/-large() exactly
	// where operator/ does, so the idivl dividend always fits in int32.
	real_t recip() const noexcept
	{
		if (raw == 0)
			return large();
#if defined(__DJGPP__) && defined(__i386__)
		// Guard exactly matches the int64 path below: 2^32/raw overflows for
		// raw in {1, 2} and underflows only for raw == -1 (raw == -2 is
		// exactly INT32_MIN and is representable).
		const uint32_t abs_raw = (raw >= 0) ? static_cast<uint32_t>(raw) : (0u - static_cast<uint32_t>(raw));
		if (raw > 0 && abs_raw <= 2u)
			return large();
		if (raw < 0 && abs_raw == 1u)
			return from_raw(-large().raw);
		int32_t result;
		__asm__("idivl %3"
						: "=a"(result)
						: "d"(1), "a"(0), "rm"(raw)
						: "cc");
		return from_raw(result);
#else
		const int64_t result64 = (int64_t(ONE) << FRAC_BITS) / int64_t(raw);
		if (result64 > INT32_MAX)
			return large();
		if (result64 < INT32_MIN)
			return from_raw(-large().raw);
		return from_raw(static_cast<int32_t>(result64));
#endif
	}

	// Scale by a plain integer: operates on raw bits directly (no extra shift).
	constexpr real_t operator*(int k) const noexcept { return from_raw(raw * k); }
	constexpr real_t operator/(int k) const noexcept { return from_raw(raw / k); }
	friend constexpr real_t operator*(int k, real_t r) noexcept { return r * k; }

	constexpr real_t &operator+=(real_t o) noexcept
	{
		raw += o.raw;
		return *this;
	}
	constexpr real_t &operator-=(real_t o) noexcept
	{
		raw -= o.raw;
		return *this;
	}
	constexpr real_t &operator*=(real_t o) noexcept
	{
		*this = *this * o;
		return *this;
	}
	real_t &operator/=(real_t o) noexcept
	{
		*this = *this / o;
		return *this;
	}
	constexpr real_t &operator*=(int k) noexcept
	{
		raw *= k;
		return *this;
	}
	constexpr real_t &operator/=(int k) noexcept
	{
		raw /= k;
		return *this;
	}

	constexpr bool operator==(real_t o) const noexcept { return raw == o.raw; }
	constexpr bool operator!=(real_t o) const noexcept { return raw != o.raw; }
	constexpr bool operator<(real_t o) const noexcept { return raw < o.raw; }
	constexpr bool operator<=(real_t o) const noexcept { return raw <= o.raw; }
	constexpr bool operator>(real_t o) const noexcept { return raw > o.raw; }
	constexpr bool operator>=(real_t o) const noexcept { return raw >= o.raw; }
};

// Floor: round toward negative infinity by zeroing fractional bits.
constexpr real_t real_floor(real_t v) noexcept
{
	return real_t::from_raw(v.raw & ~(real_t::ONE - 1));
}

constexpr real_t real_abs(real_t v) noexcept
{
	return v.raw >= 0 ? v : -v;
}

// Precomputed trig tables (1024 entries, ~8 KB total).
// real_trig_init() must be called once at program startup before any
// real_sin / real_cos call.
static constexpr int TRIG_TABLE_SIZE = 1024;

inline real_t _sin_table[TRIG_TABLE_SIZE];
inline real_t _cos_table[TRIG_TABLE_SIZE];

inline void real_trig_init() noexcept
{
	for (int i = 0; i < TRIG_TABLE_SIZE; ++i)
	{
		const float a = float(i) * (2.0f * 3.14159265358979f / float(TRIG_TABLE_SIZE));
		_sin_table[i] = real_t(std::sin(a));
		_cos_table[i] = real_t(std::cos(a));
	}
}

// Convert a radian angle (Q16.16) to a table index in [0, TRIG_TABLE_SIZE).
inline int _trig_index(real_t angle) noexcept
{
	// TWO_PI_RAW = round(2π × ONE) = round(2π × 65536) = 411775
	static constexpr int64_t TWO_PI_RAW = 411775LL;
	int64_t idx = (static_cast<int64_t>(angle.raw) * TRIG_TABLE_SIZE) / TWO_PI_RAW;
	idx %= TRIG_TABLE_SIZE;
	if (idx < 0)
		idx += TRIG_TABLE_SIZE;
	return static_cast<int>(idx);
}

inline real_t real_sin(real_t angle) noexcept { return _sin_table[_trig_index(angle)]; }
inline real_t real_cos(real_t angle) noexcept { return _cos_table[_trig_index(angle)]; }
