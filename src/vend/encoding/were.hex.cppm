module;

#include <string_view>
#include <string>
#include <expected>
#include <vector>
#include <span>
#include <cassert>
#include <ranges>
#include <array>

export module were.hex;
import weretype;

export namespace were::hex {
	
	// [Constants]
	constexpr std::string_view Legend{"0123456789ABCDEF"};
	enum struct Error { BadLength, BadChar};
	
	constexpr int CaseOffset = 'a'-'A';
	constexpr auto NormalizedHex = [](char ch) -> char { return ch >= 'a' && ch <= 'f' ? ch - CaseOffset : ch; };


	// [Functions]
	[[nodiscard]] constexpr auto numeric(char ch) -> std::expected<u8, Error> {
		const auto pos = Legend.find(NormalizedHex(ch));
		if (pos == std::string_view::npos) return std::unexpected(Error::BadChar);
		return as<u8>(pos);
	}

	[[nodiscard]] constexpr auto decode(std::string_view input) -> std::expected<std::vector<u8>, Error> {
		if (input.size() % 2) return std::unexpected(Error::BadLength);
		std::vector<u8> out;
		out.reserve(input.size() / 2);

		for (auto pair : input | std::views::chunk(2)) {
			auto high = numeric(pair[0]);
			auto low  = numeric(pair[1]);
			if (!high || !low) return std::unexpected(Error::BadChar);

			 out.push_back((*high << 4) | *low);
		}
		return out;
	}

	[[nodiscard]] constexpr auto encode(std::span<const u8> input) -> std::string {
		std::string output(input.size() * 2, '\0');

		for (auto [i, value] : were::thru(input)) {
			output[i * 2]     = Legend[value >> 4];
			output[i * 2 + 1] = Legend[value & 0x0F];
		}

		return output;
	}
}

// [Static Assert Testing]
namespace were::hex {
	static_assert(Legend.size() == 16, "Hex legend must contain exactly 16 characters");

	static_assert([] {
		constexpr std::array<u8, 3> input{0x12, 0xAB, 0xFF};
		return encode(input) == "12ABFF";
	}(), "Hex encode test failed");;


	static_assert([] {
		auto result = decode("12ABFF");
		if (!result) return false;
		return *result == std::vector<u8>{0x12, 0xAB, 0xFF};
	}(), "Hex decode test failed");

	// invalid input test
	static_assert([] {
		auto result = decode("GG");
		return !result && result.error() == Error::BadChar;
	}(), "Invalid decode input test failed");
}
