module;

#include <cstddef>
#include <cstdint>
#include <array>
#include <limits>
#include <string>
#include <ranges>
#include <expected>
#include <span>
#include <utility>

export module weretype;

export {

	// [DATA ALIASES]
	using byte = std::uint8_t;

	using u8 = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using i8 = std::int8_t;
	using i16 = std::int16_t;
	using i32 = std::int32_t;
	using i64 = std::int64_t;

	using f32 = float;
	using f64 = double;

	// as<T>(V) -- static_cast alias at compile time
	template <typename ToType, typename From>
	[[nodiscard]] constexpr ToType as(From&& value)
		noexcept(noexcept(static_cast<ToType>(std::forward<From>(value)))) {
		return static_cast<ToType>(std::forward<From>(value));
	}

	// raw<T>(V) -- runtime reinterpret_cast alias
	template <typename ToType, typename From>
	[[nodiscard]] constexpr ToType raw(From&& value) {
		return reinterpret_cast<ToType>(value);
	}

	// u8span() -- reads anything as a std::span<const u8>
	template<class T>
	concept byteRange = std::ranges::contiguous_range<const T> && std::ranges::sized_range<const T>;

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

	// Alias for std::expected<T,E>
	template <typename T = void>
	using Result = std::expected<T, std::string>;

	// thru(count) -- indices from zero up to, but excluding, count.
	[[nodiscard]] constexpr auto thru(std::size_t count) noexcept {
		return std::views::iota(0uz, count);
	}

	// thru(R) -- Enumerate View -- wrapper for std::views::enumerate
	#if !defined(__cpp_lib_ranges_enumerate) && defined(__cpp_lib_ranges_zip)
		template <std::ranges::viewable_range R>
		constexpr auto thru(R&& range) {
			return std::views::zip(std::views::iota(0uz), std::forward<R>(range));
		}
	#else
		// Standard C++ implementation
		template <std::ranges::viewable_range R>
		constexpr auto thru(R&& range) {
			return std::views::enumerate(std::forward<R>(range));
		}
	#endif
}