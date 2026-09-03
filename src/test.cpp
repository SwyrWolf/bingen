#include <print>

extern "C" int square(int value);

int main() {
	std::println("{}", square(3));
}
