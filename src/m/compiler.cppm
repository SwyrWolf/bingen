module;

#include <string_view>
#include <string>
#include <expected>
#include <array>
#include <utility>
#include <vector>
#include <span>
#include <bit>
#include <algorithm>
#include <filesystem>
#include <fstream>

export module compiler;
import weretype;
import were.hex;

namespace compiler {
	constexpr std::array<std::pair<std::string_view, std::size_t>, 3> wordSizes{{
		{"SW", 2},
		{"DW", 4},
		{"QW", 8},
	}};

	export constexpr bool is_word_size(std::string_view marker) {
		for (const auto& [word, size] : wordSizes)
			if (word == marker) return true;
		return false;
	}

  constexpr std::string_view syntax_characters{"{}\""};
	constexpr std::string_view whitespace{" \t\r\n"};
  constexpr std::string_view line_comment{"//"};

	export enum class token_type {
		word_size, hex_byte, string_literal, open_brace, close_brace
	};

	export struct token {
		token_type type{};
		std::string_view text{};
	};

	export constexpr auto tokenize(std::string_view input)
		-> std::expected<std::vector<token>, std::string_view> {
		std::vector<token> tokens{};

		while (!input.empty()) {
			if (whitespace.find(input.front()) != whitespace.npos) {
				input.remove_prefix(1);
				continue;
			}
			if (input.starts_with(line_comment)) {
				const auto end{input.find('\n')};
				input.remove_prefix(end == input.npos ? input.size() : end + 1);
				continue;
			}
			if (input.front() == '{' || input.front() == '}') {
				tokens.push_back({input.front() == '{' ? token_type::open_brace : token_type::close_brace,
				                  input.substr(0, 1)});
				input.remove_prefix(1);
				continue;
			}
			if (input.front() == '"') {
				const auto end{input.find('"', 1)};
				if (end == input.npos) return std::unexpected{"Unterminated string"};
				tokens.push_back({token_type::string_literal, input.substr(1, end - 1)});
				input.remove_prefix(end + 1);
				continue;
			}

			const auto marker{input.substr(0, 2)};
			if (is_word_size(marker)) {
				tokens.push_back({token_type::word_size, marker});
			} else if (marker.size() == 2 && were::hex::numeric(marker[0]) && were::hex::numeric(marker[1])) {
				tokens.push_back({token_type::hex_byte, marker});
			} else {
				return std::unexpected{"Expected word size, hex byte, brace, or string"};
			}
			input.remove_prefix(2);
		}
		return tokens;
	}

	constexpr auto decode_byte(std::string_view text) -> std::expected<u8, std::string_view> {
		if (text.size() != 2) return std::unexpected{"Expected a two-digit hex byte"};
		const auto high{were::hex::numeric(text[0])};
		const auto low{were::hex::numeric(text[1])};
		if (!high || !low) return std::unexpected{"Invalid hex byte"};
		return as<u8>((*high << 4) | *low);
	}

	// Token text borrows the source, which must remain alive during compilation.
	export constexpr auto compile(std::span<const token> tokens)
		-> std::expected<std::vector<u8>, std::string_view> {
			
		static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);

		std::vector<u8> output{};
		std::size_t position{};
		while (position < tokens.size()) {
			const auto initializer{tokens[position++]};
			const bool word{initializer.type == token_type::word_size};
			std::size_t size{};
			if (word) {
				for (const auto& [marker, width] : wordSizes)
					if (initializer.text == marker) size = width;
				if (!size) return std::unexpected{"Unknown word size"};
			} else if (initializer.type == token_type::hex_byte) {
				const auto count{decode_byte(initializer.text)};
				if (!count) return std::unexpected{count.error()};
				size = *count;
			} else {
				return std::unexpected{"Expected a byte count or word size"};
			}
			if (position == tokens.size() || tokens[position++].type != token_type::open_brace)
				return std::unexpected{"Expected opening brace"};
			const auto start{output.size()};
			while (position < tokens.size() && tokens[position].type != token_type::close_brace) {
				const auto item{tokens[position++]};
				if (item.type == token_type::hex_byte) {
					const auto value{decode_byte(item.text)};
					if (!value) return std::unexpected{value.error()};
					if (output.size() - start == size) return std::unexpected{"Initializer exceeds declared size"};
					output.push_back(*value);
				} else if (!word && item.type == token_type::string_literal) {
					if (item.text.size() > size - (output.size() - start))
						return std::unexpected{"Initializer exceeds declared size"};
					for (char ch : item.text) output.push_back(as<u8>(ch));
				} else {
					return std::unexpected{"Expected hex bytes (or strings in a raw initializer)"};
				}
			}
			if (position == tokens.size()) return std::unexpected{"Expected closing brace"};
			++position;
			if (output.size() - start != size) return std::unexpected{"Initializer does not match declared size"};
			// Word literals are written most-significant byte first.
			if (word && std::endian::native == std::endian::little)
				std::reverse(output.end() - as<std::ptrdiff_t>(size), output.end());
		}
		return output;
	}

	export constexpr auto compile(std::string_view input)
		-> std::expected<std::vector<u8>, std::string_view> {
		const auto tokens{tokenize(input)};
		if (!tokens) return std::unexpected{tokens.error()};
		return compile(std::span<const token>{*tokens});
	}

	export auto compile(std::string_view input, const std::filesystem::path& path) -> were::Result<> {
		const auto bytes{compile(input)};
		if (!bytes) return std::unexpected{std::string{bytes.error()}};
		if (!std::in_range<std::streamsize>(bytes->size()))
			return std::unexpected{"Binary output is too large"};
		std::ofstream file{path, std::ios::binary | std::ios::trunc};
		if (!file) return std::unexpected{"Could not open output file: " + path.string()};
		if (!bytes->empty()) file.write(raw<const char*>(bytes->data()), as<std::streamsize>(bytes->size()));
		file.close();
		if (!file) return std::unexpected{"Could not write output file: " + path.string()};
		return {};
	}
}

// [Static Assert Testing]
namespace compiler::static_tests {
	static_assert([] {
		const auto tokens{tokenize("04{\"A\" 00 ab FF}SW{12 34}DW{01 23 45 67}QW{01 23 45 67 89 ab cd ef}00{}")};
		if (!tokens) return false;
		const auto result{compile(std::span<const token>{*tokens})};
		const std::vector<u8> expected{std::endian::native == std::endian::little
			? std::vector<u8>{0x41, 0, 0xAB, 0xFF, 0x34, 0x12, 0x67, 0x45, 0x23, 0x01,
				0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01}
			: std::vector<u8>{0x41, 0, 0xAB, 0xFF, 0x12, 0x34, 0x01, 0x23, 0x45, 0x67,
				0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}};
		return result && *result == expected;
	}(), "Raw bytes and native-endian words must compile from tokenizer output");

	static_assert([] {
		for (const auto input : {"SW", "SW{00}", "SW{00 00 00}", "01{00", "01 00}",
			"01{SW{00 00}}", "SW{\"ab\"}", "00{00}", "01{\"ab\"}", "}", "GG", "01{\"a}"})
			if (compile(std::string_view{input})) return false;
		const auto empty{compile(std::string_view{"// empty\n"})};
		return empty && empty->empty();
	}(), "Malformed initializers must fail and empty input must compile");

	static_assert([] {
		const auto result{tokenize("SW")};
		return result && result->size() == 1 && result->front().type == token_type::word_size;
	}(), "SW should produce a word-size token");

	static_assert([] {
		const auto result{tokenize(" \t\r\n")};
		return result && result->empty();
	}(), "Whitespace should produce no tokens");
}
