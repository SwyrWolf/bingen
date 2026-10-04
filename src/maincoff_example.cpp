import weretype;
import coff;

#include <array>
#include <bit>
#include <concepts>
#include <fstream>
#include <utility>
#include <vector>

template <std::unsigned_integral T>
void appendLittleEndian(std::vector<u8>& output, T value) {
	static_assert(
		std::endian::native == std::endian::little
		|| std::endian::native == std::endian::big
	);

	if constexpr (std::endian::native == std::endian::big) {
		value = std::byteswap(value);
	}

	output.append_range(std::bit_cast<std::array<u8, sizeof(T)>>(value));
}

int main() {
	const COFF::Header header{
		.machine = COFF::Machine_e::x86_64,
		.numberOfSections = 1,
		.pointerToSymbolTable = 0x42,
		.numberOfSymbols = 1,
	};

	const COFF::Section section{
		.header = COFF::SectionHeader{
			.name = { '.', 't', 'e', 'x', 't', 0, 0, 0 },
			.sizeOfRawData = 6,
			.pointerToRawData = 0x3c,
			.characteristics = COFF::SectionCharacteristics(
				COFF::SectionContent_e::ContainsCode,
				COFF::SectionAlignment_e::Align16Bytes,
				COFF::SectionMemory_e::MemoryExecute,
				COFF::SectionMemory_e::MemoryRead
			).value,
		},
		.data = { 0x89, 0xc8, 0x0f, 0xaf, 0xc1, 0xc3 },
	};

	const COFF::Symbol symbol{
		.name = { 's', 'q', 'u', 'a', 'r', 'e', 0, 0 },
		.sectionNumber = 1,
		.type = 0x20,
		.storageClass = 2,
	};

	const COFF::StringTable stringTable{ .totalSize = 4 };

	std::vector<u8> encoded;
	encoded.reserve(20 + 40 + section.data.size() + sizeof(COFF::Symbol) + 4);

	appendLittleEndian(encoded, std::to_underlying(header.machine));
	appendLittleEndian(encoded, header.numberOfSections);
	appendLittleEndian(encoded, header.timeDateStamp);
	appendLittleEndian(encoded, header.pointerToSymbolTable);
	appendLittleEndian(encoded, header.numberOfSymbols);
	appendLittleEndian(encoded, header.sizeOfOptionalHeader);
	appendLittleEndian(encoded, header.characteristics);

	encoded.append_range(section.header.name);
	appendLittleEndian(encoded, section.header.virtualSize);
	appendLittleEndian(encoded, section.header.virtualAddress);
	appendLittleEndian(encoded, section.header.sizeOfRawData);
	appendLittleEndian(encoded, section.header.pointerToRawData);
	appendLittleEndian(encoded, section.header.pointerToRelocations);
	appendLittleEndian(encoded, section.header.pointerToLineNumbers);
	appendLittleEndian(encoded, section.header.numberOfRelocations);
	appendLittleEndian(encoded, section.header.numberOfLineNumbers);
	appendLittleEndian(encoded, section.header.characteristics);

	encoded.append_range(section.data);
	encoded.append_range(symbol.name);
	appendLittleEndian(encoded, symbol.value);
	appendLittleEndian(encoded, as<u16>(symbol.sectionNumber));
	appendLittleEndian(encoded, symbol.type);
	encoded.push_back(symbol.storageClass);
	encoded.push_back(symbol.numberOfAuxRecords);
	appendLittleEndian(encoded, stringTable.totalSize);

	std::ofstream output("square.o", std::ios::binary);
	output.write(raw<const char*>(encoded.data()), encoded.size());
	return output ? 0 : 1;
}
