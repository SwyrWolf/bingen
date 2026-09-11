module;

#include <cstdint>

export module divdec;
import weretype;

export namespace DivDec {

	struct UnsignedResult {
		u64 quotient;
		u64 remainder;
	};

	struct SignedResult {
		i64 quotient;
		i64 remainder;
	};

	// Exact unsigned division by 10 using the reciprocal ceil(2^67 / 10).
	// The quotient is the upper 64 bits of n * 0xCCCCCCCCCCCCCCCD, shifted by 3.
	// Split the multiplication into 32-bit limbs so no 128-bit type is needed.
	constexpr UnsignedResult divideUnsigned(u64 number) noexcept {
		constexpr u64 mask = 0xFFFFFFFFull;
		constexpr u64 lowMultiplier = 0xCCCCCCCDull;
		constexpr u64 highMultiplier = 0xCCCCCCCCull;
		const auto low = number & mask;
		const auto high = number >> 32;

		const auto lowProduct = low * lowMultiplier;
		const auto middle = high * lowMultiplier + (lowProduct >> 32);
		const auto carry = (middle & mask) + low * highMultiplier;
		const auto upper = high * highMultiplier + (middle >> 32) + (carry >> 32);
		const auto quotient = upper >> 3;
		return { quotient, number - quotient * 10 };
	}

	// Match C++ signed division: truncate toward zero; remainder follows the sign.
	constexpr SignedResult divideSigned(i64 number) noexcept {
		const bool negative = number < 0;
		const auto bits = as<u64>(number);
		// Unsigned subtraction safely computes the magnitude even for INT64_MIN.
		const auto magnitude = negative ? 0uz - bits : bits;
		const auto result = divideUnsigned(magnitude);
		// After division by 10, both values fit in a signed 64-bit integer.
		const auto quotient = as<i64>(result.quotient);
		const auto remainder = as<i64>(result.remainder);
		return { negative ? -quotient : quotient, negative ? -remainder : remainder };
	}
}
