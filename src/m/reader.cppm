module;
#include <expected>
#include <filesystem>
#include <fstream>

export module reader;
import weretype;

export namespace reader {
	[[nodiscard]] bool validatePath(const std::filesystem::path& path) {
		return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
	}

	[[nodiscard]] std::string readFile(const std::filesystem::path& path) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file) return {};

		const auto size = file.tellg();
		file.seekg(0);

		std::string contents(as<std::size_t>(size), '\0');
		file.read(contents.data(), size);

		return contents;
	}
}