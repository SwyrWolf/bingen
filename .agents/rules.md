Coding conventions

* Prefer value initialization with {} for zero/default initialization.
* Write int value{}; instead of int value = 0;.
* Prefer the fixed-width numeric aliases from weretype instead of the standard-library fixed-width integer types.
* Use u8, u16, u32, and u64 instead of std::uint8_t, std::uint16_t, std::uint32_t, and std::uint64_t.
* Use i8, i16, i32, and i64 instead of the corresponding std::int*_t types.
* Use f32 and f64 instead of float and double where fixed-width floating-point aliases are appropriate.
* Prefer as<T>(value) instead of static_cast<T>(value).
* Prefer raw<T>(value) instead of reinterpret_cast<T>(value).