module;

#include <ranges>
#include <span>
#include <utility>

export module weretype;

export {

	// [DATA ALIASES]
	using byte = unsigned char;

	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	using u64 = unsigned long long;

	using i8 = signed char;
	using i16 = signed short;
	using i32 = signed int;
	using i64 = signed long long;

	using f32 = float;
	using f64 = double;

	// as<T>(V) -- static_cast alias at compile time
	template <typename ToType, typename From>
	[[nodiscard]] constexpr ToType as(From&& value) {
		return static_cast<ToType>(value);
	}

	// raw<T>(V) -- reinterpret_cast alias at compile time
	template <typename ToType, typename From>
	[[nodiscard]] constexpr ToType raw(From&& value) {
		return reinterpret_cast<ToType>(value);
	}

	// u8span() -- reads anything as a std::span<const u8>
	template<class T>
	concept byteRange =
		std::ranges::contiguous_range<T>
		&& std::ranges::sized_range<T>;

	template<byteRange T>
	[[nodiscard]] auto u8span(const T& input)
	-> std::span<const u8> {
		using Elem = std::ranges::range_value_t<T>;

		return {
			raw<const u8*>(std::ranges::data(input)),
			std::ranges::size(input) * sizeof(Elem)
		};
	}
}


export namespace were {
	// thru(R) -- Enumerate View -- wrapper for std::views::enumerate
	#if defined(__APPLE__) || defined(__MACH__)
		// fallback for Apple w/ LLVM Clang compiler and libc++ support
		template <typename R>
		constexpr auto thru(R&& range) {
						return std::views::zip(std::views::iota(0uz), std::forward<R>(range));
		}
	#else
		// Standard C++ implementation
		template <typename R>
		constexpr auto thru(R&& range) {
			return std::views::enumerate(std::forward<R>(range));
		}
	#endif
}