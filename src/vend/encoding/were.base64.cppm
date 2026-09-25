module;

#include <string_view>
#include <string>
#include <expected>
#include <vector>
#include <span>
#include <ranges>
#include <array>

export module were.base64;
import weretype;

export namespace were::base64 {

	// [Constants]
	constexpr std::string_view Legend{
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/"
	};
	enum struct Error { BadLength, BadChar, BadPadding };

	[[nodiscard]] constexpr auto numeric(char ch) -> std::expected<u8, Error> {
		const auto pos = Legend.find(ch);
		if (pos == std::string_view::npos) return std::unexpected(Error::BadChar);

    return as<u8>(pos);
	}

	[[nodiscard]] constexpr auto decode(std::string_view input) -> std::expected<std::vector<u8>, Error> {
		if (input.size() % 4 || input.empty()) return std::unexpected(Error::BadLength);

		const auto padding = std::ranges::distance(
			input 
			| std::views::reverse 
			| std::views::take_while([](char ch) { return ch == '='; })
		);
		if (padding > 2) return std::unexpected(Error::BadPadding);

		for (auto i : were::thru(input.size() - padding)) {
			if (input[i] == '=') return std::unexpected(Error::BadPadding);
		}

		std::vector<u8> out;
		out.reserve((input.size() / 4) * 3 - padding);

		for (auto i : were::thru(input.size()) | std::views::stride(4)) {
			u8 sextets[4]{};

			for (auto x : were::thru(4)) {
				const char ch = input[i + x];
				if (ch == '=') {
					sextets[x] = 0;
					continue;
				}

				auto r = numeric(ch);
				if (!r) return std::unexpected(r.error());

				sextets[x] = *r;
			}

			const u8 b0 = as<u8>((sextets[0] << 2) | (sextets[1] >> 4));
			const u8 b1 = as<u8>(((sextets[1] & 0x0F) << 4) | (sextets[2] >> 2));
			const u8 b2 = as<u8>(((sextets[2] & 0x03) << 6) | sextets[3]);

			out.push_back(b0);
			if (input[i + 2] != '=') out.push_back(b1);
			if (input[i + 3] != '=') out.push_back(b2);
		}

		return out;
	}

	[[nodiscard]] constexpr auto encode(std::span<const u8> input) -> std::string {
		if (input.empty()) return {};

		const auto [fullGroups, remainder] = std::pair{ input.size() / 3, input.size() % 3 };

		std::string out;
		out.resize(((input.size() + 2) / 3) * 4);

		for (const auto [indx, group] :
			were::thru(
				input
				| std::views::take(fullGroups * 3)
				| std::views::chunk(3)
			)) {
			const u32 value =
				(as<u32>(group[0]) << 16) |
				(as<u32>(group[1]) <<  8) |
				as<u32>(group[2]);

			for (const auto i : were::thru(4)) {
				out[indx * 4 + i] = Legend[(value >> (18 - i * 6)) & 0b0011'1111];
			}
		}

		const auto inBase  = fullGroups * 3;
		const auto outBase = fullGroups * 4;

		if (remainder) {
			u32 value{};

			for (const auto [i, byte] : were::thru(input | std::views::drop(inBase))) {
				value |= as<u32>(byte) << (16 - i * 8);
			}

			out[outBase + 0] = Legend[value >> 18];
			out[outBase + 1] = Legend[(value >> 12) & 0x3F];
			out[outBase + 2] = Legend[(value >>  6) & 0x3F];
			out[outBase + 3] = Legend[value & 0x3F];

			for (const auto i : were::thru(3 - remainder))
				out[out.size() - 1 - i] = '=';
		}

		return out;
	}
}

// [Static Assert Testing]
namespace were::base64 {
	static_assert(Legend.size() == 64, "Base64 legend must contain exactly 64 characters");

	static_assert([] {
		constexpr std::array<u8, 3> input{'f', 'o', 'o'};
		return encode(input) == "Zm9v"
			&& encode(std::span{input}.first(2)) == "Zm8="
			&& encode(std::span{input}.first(1)) == "Zg=="
			&& encode(std::span<const u8>{}).empty();
	}(), "Base64 encode test failed");

	static_assert([] {
		auto full = decode("Zm9v");
		auto onePadding = decode("Zm8=");
		auto twoPadding = decode("Zg==");
		return full && *full == std::vector<u8>{'f', 'o', 'o'}
			&& onePadding && *onePadding == std::vector<u8>{'f', 'o'}
			&& twoPadding && *twoPadding == std::vector<u8>{'f'};
	}(), "Base64 decode test failed");

	static_assert([] {
		constexpr std::array<u8, 3> input{0x00, 0xFB, 0xFF};
		auto result = decode("APv/");
		return encode(input) == "APv/"
			&& result && *result == std::vector<u8>{0x00, 0xFB, 0xFF};
	}(), "Base64 binary data test failed");

	static_assert([] {
		for (auto input : {"", "Zg", "Zg==="}) {
			auto result = decode(input);
			if (result || result.error() != Error::BadLength) return false;
		}
		return true;
	}(), "Base64 invalid length test failed");

	static_assert([] {
		auto result = decode("Zm9!");
		return !result && result.error() == Error::BadChar;
	}(), "Base64 invalid character test failed");

	static_assert([] {
		for (auto input : {"Z===", "====", "Z=g=", "Zg==Zm9v"}) {
			auto result = decode(input);
			if (result || result.error() != Error::BadPadding) return false;
		}
		return true;
	}(), "Base64 invalid padding test failed");
}
