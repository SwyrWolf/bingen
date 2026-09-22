module;

#include <string>
#include <expected>

export module compiler;

export namespace compiler{

	[[nodiscard]]
	auto parse(std::string_view data) -> std::expected<std::string, std::string> {
	
	}
}
