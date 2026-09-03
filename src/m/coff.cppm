module;

#include <array>
#include <string_view>

export module coff;
import weretype;

export namespace COFF {

	enum struct Machine_e : u16 {
		Unknown = 0x0000,
		x86_64 = 0x8664, //AKA AMD64
		arm_64 = 0xaa64,
		i386 = 0x014c,
		r3000 = 0x0162,
		r4000 = 0x0166,
		r10000 = 0x0168,
		wcemipsV2 = 0x0169,
		alpha = 0x0184,
		sh3 = 0x01a2,
		sh3Dsp = 0x01a3,
		sh3E = 0x01a4,
		sh4 = 0x01a6,
		sh5 = 0x01a8,
		arm = 0x01c0,
		thumb = 0x01c2,
		armNt = 0x01c4,
		am33 = 0x01d3,
		powerPc = 0x01f0,
		powerPcFp = 0x01f1,
		ia64 = 0x0200,
		mips16 = 0x0266,
		alpha64 = 0x0284,
		mipsFpu = 0x0366,
		mipsFpu16 = 0x0466,
		ebc = 0x0ebc,
		m32R = 0x9041,
		cee = 0xc0ee,
		arm64Ec = 0xa641,
		arm64X = 0xa64e,
		chpeX86 = 0x3a64,
		riscV32 = 0x5032,
		riscV64 = 0x5064,
		riscV128 = 0x5128,
		loongArch32 = 0x6232,
		loongArch64 = 0x6264,
	};

	// Section characteristics

	enum struct SectionContent_e : u8 {
		TypeNoPad = 1u << 3,
		ContainsCode = 1u << 5,
		ContainsInitializedData = 1u << 6,
		ContainsUninitializedData = 1u << 7,
	};

	enum struct SectionLinker_e : u32 {
		LinkerOther = 1u << 8,
		LinkerInfo = 1u << 9,
		LinkerRemove = 1u << 11,
		Comdat = 1u << 12,
		NoDeferSpecExc = 1u << 14,
		GpRel = 1u << 15,
		ExtendedRelocationOverflow = 1u << 24,
	};

	enum struct SectionAlignment_e : u32 {
		Align1Bytes = 1u << 20,
		Align2Bytes = 2u << 20,
		Align4Bytes = 3u << 20,
		Align8Bytes = 4u << 20,
		Align16Bytes = 5u << 20,
		Align32Bytes = 6u << 20,
		Align64Bytes = 7u << 20,
		Align128Bytes = 8u << 20,
		Align256Bytes = 9u << 20,
		Align512Bytes = 10u << 20,
		Align1024Bytes = 11u << 20,
		Align2048Bytes = 12u << 20,
		Align4096Bytes = 13u << 20,
		Align8192Bytes = 14u << 20,
	};

	enum struct SectionMemory_e : u32 {
		MemoryPurgeable = 1u << 17,
		MemoryLocked = 1u << 18,
		MemoryPreload = 1u << 19,
		MemoryDiscardable = 1u << 25,
		MemoryNotCached = 1u << 26,
		MemoryNotPaged = 1u << 27,
		MemoryShared = 1u << 28,
		MemoryExecute = 1u << 29,
		MemoryRead = 1u << 30,
		MemoryWrite = 1u << 31,
	};

	struct SectionCharacteristics {
		u32 value{};

		constexpr SectionCharacteristics() = default;

		template <typename... Characteristic>
		requires (sizeof...(Characteristic) > 0)
		constexpr SectionCharacteristics(Characteristic... characteristic)
			: value((as<u32>(characteristic) | ...)) {}
	};

	// Section metadata

	struct SectionInfo {
		std::string_view name;
		std::string_view description;
	};

	inline constexpr auto sectionInfo = std::to_array<SectionInfo>({
		{ ".text", "Executable code" },
		{ ".bss", "Uninitialized data" },
		{ ".rdata", "Read-only data" },
		{ ".data", "Initialized data" },
		{ ".pdata", "Exception handling data" },
		{ ".xdata", "Exception handling unwind data" },
		{ ".idata", "Import data" },
		{ ".edata", "Export data" },
		{ ".reloc", "Base relocations" },
		{ ".debug", "Debugging information" },
		{ ".rsrc", "Resource data" },
		{ ".tls", "Thread-local storage data" },
		{ ".crt", "C runtime data" },
		{ ".cormeta", "CLR metadata" },
	});

}

