
#include <bit>
#include <cstdio>
#include <limits>

import divdec;
import weretype;

static_assert(DivDec::divideUnsigned(123).quotient == 12);
static_assert(DivDec::divideSigned(-123).remainder == -3);

int main() {
	const auto check = [](u64 value) {
		const auto u = DivDec::divideUnsigned(value);
		const auto signedValue = std::bit_cast<i64>(value);
		const auto s = DivDec::divideSigned(signedValue);
		return u.quotient == value / 10 && u.remainder == value % 10
			&& s.quotient == signedValue / 10 && s.remainder == signedValue % 10;
	};

	// Exercise small values and both ends of the unsigned and signed ranges.
	for (const auto i : were::thru(10000uz)) {
		if (!check(i) || !check(std::numeric_limits<u64>::max() - i)
			|| !check((u64{1} << 63) + i)
			|| !check((u64{1} << 63) - i)) {
			std::fprintf(stderr, "Boundary iteration %zu failed\n", i);
			return 1;
		}
	}

	// Spread values across all 64 bits, comparing with native / and %.
	u64 state = 0x123456789ABCDEF0ull;
	for ([[maybe_unused]] const auto i : were::thru(1000000uz)) {
		state ^= state << 13;
		state ^= state >> 7;
		state ^= state << 17;
		if (!check(state)) return 1;
	}
}
