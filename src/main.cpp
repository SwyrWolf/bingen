import weretype;
import coff;

#include <fstream>
#include <vector>

void appendU16(std::vector<u8>& output, u16 value) {
	output.push_back(as<u8>(value));
	output.push_back(as<u8>(value >> 8));
}

void appendU32(std::vector<u8>& output, u32 value) {
	appendU16(output, as<u16>(value));
	appendU16(output, as<u16>(value >> 16));
}

template <typename T, std::size_t Size>
void appendBytes(std::vector<u8>& output, const std::array<T, Size>& bytes) {
	for (const auto byte : bytes) {
		output.push_back(as<u8>(byte));
	}
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

	appendU16(encoded, as<u16>(header.machine));
	appendU16(encoded, header.numberOfSections);
	appendU32(encoded, header.timeDateStamp);
	appendU32(encoded, header.pointerToSymbolTable);
	appendU32(encoded, header.numberOfSymbols);
	appendU16(encoded, header.sizeOfOptionalHeader);
	appendU16(encoded, header.characteristics);

	appendBytes(encoded, section.header.name);
	appendU32(encoded, section.header.virtualSize);
	appendU32(encoded, section.header.virtualAddress);
	appendU32(encoded, section.header.sizeOfRawData);
	appendU32(encoded, section.header.pointerToRawData);
	appendU32(encoded, section.header.pointerToRelocations);
	appendU32(encoded, section.header.pointerToLineNumbers);
	appendU16(encoded, section.header.numberOfRelocations);
	appendU16(encoded, section.header.numberOfLineNumbers);
	appendU32(encoded, section.header.characteristics);

	encoded.insert(encoded.end(), section.data.begin(), section.data.end());
	appendBytes(encoded, symbol.name);
	appendU32(encoded, symbol.value);
	appendU16(encoded, as<u16>(symbol.sectionNumber));
	appendU16(encoded, symbol.type);
	encoded.push_back(symbol.storageClass);
	encoded.push_back(symbol.numberOfAuxRecords);
	appendU32(encoded, stringTable.totalSize);

	std::ofstream output("square.o", std::ios::binary);
	output.write(raw<const char*>(encoded.data()), encoded.size());
	return output ? 0 : 1;
}