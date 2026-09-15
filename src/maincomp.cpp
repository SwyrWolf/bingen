#include <print>
#include <filesystem>

import weretype;
import reader;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::printf("No arguments given!");
		return 0;
	}

	if (reader::validatePath(argv[1])) {
		std::string contents = reader::readFile(argv[1]);
		std::println("{}", contents);
	} else {
		std::printf("Invalid File Path!");
		return 0;
	}
}