module;
#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <utility>

export module reader;
import weretype;

export namespace reader {
	[[nodiscard]] bool validatePath(const std::filesystem::path& path) {
		std::error_code error;
		return std::filesystem::is_regular_file(path, error);
	}

	[[nodiscard]] auto readFile(const std::filesystem::path& path) -> were::Result<std::string> {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file) return std::unexpected("Could not open input file: " + path.string());

		const auto size = as<std::streamoff>(file.tellg());
		if (size < 0 || !std::in_range<std::size_t>(size) || !std::in_range<std::streamsize>(size))
			return std::unexpected("Could not determine input file size: " + path.string());
		file.seekg(0);
		if (!file) return std::unexpected("Could not seek input file: " + path.string());

		std::string contents(as<std::size_t>(size), '\0');
		if (!file.read(contents.data(), as<std::streamsize>(size)))
			return std::unexpected("Could not read input file: " + path.string());

		return contents;
	}
}
