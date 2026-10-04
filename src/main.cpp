#include <print>
#include <filesystem>
#include <cstdio>
#include <string>
#include <system_error>

import weretype;
import reader;
import compiler;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::println(stderr, "Usage: bingen <input.bingen> [output.bin]");
		return 1;
	}

	if (reader::validatePath(argv[1])) {
		auto contents = reader::readFile(argv[1]);
		if (!contents) {
			std::println(stderr, "{}", contents.error());
			return 1;
		}
		auto output = argc > 2 ? std::filesystem::path(argv[2]) : std::filesystem::path(argv[1]);
		if (argc <= 2) output.replace_extension(".bin");
		std::error_code error;
		if (std::filesystem::equivalent(argv[1], output, error)) {
			std::println(stderr, "Input and output must be different files");
			return 1;
		}
		auto result = compiler::compile(*contents, output);
		if (!result) {
			std::println(stderr, "{}", result.error());
			return 1;
		}
	} else {
		std::println(stderr, "Invalid input file path: {}", argv[1]);
		return 1;
	}
}
