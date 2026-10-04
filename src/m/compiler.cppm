module;

#include <string_view>
#include <string>
#include <expected>
#include <array>
#include <utility>
#include <vector>

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
}

// [Static Assert Testing]
namespace compiler::static_tests {
	static_assert([] {
		const auto result{tokenize("SW")};
		return result && result->size() == 1 && result->front().type == token_type::word_size;
	}(), "SW should produce a word-size token");

	static_assert([] {
		const auto result{tokenize(" \t\r\n")};
		return result && result->empty();
	}(), "Whitespace should produce no tokens");
}
