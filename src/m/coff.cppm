module;

export module coff;
import weretype;

export namespace COFF {

	enum struct Machine_e : u16 {
		Unknown = 0x0000,
		x86_64 = 0x8664, //AKA AMD64
		arm_64 = 0xaa64, //Arm_64
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

	enum struct SectionCharacteristics_e : u32 {
		TypeNoPad = 0x00000008,
		ContainsCode = 0x00000020,
		ContainsInitializedData = 0x00000040,
		ContainsUninitializedData = 0x00000080,
		LinkerOther = 0x00000100,
		LinkerInfo = 0x00000200,
		LinkerRemove = 0x00000800,
		Comdat = 0x00001000,
		NoDeferSpecExc = 0x00004000,
		GpRel = 0x00008000,
		MemoryPurgeable = 0x00020000,
		Memory16Bit = 0x00020000,
		MemoryLocked = 0x00040000,
		MemoryPreload = 0x00080000,
		Align1Bytes = 0x00100000,
		Align2Bytes = 0x00200000,
		Align4Bytes = 0x00300000,
		Align8Bytes = 0x00400000,
		Align16Bytes = 0x00500000,
		Align32Bytes = 0x00600000,
		Align64Bytes = 0x00700000,
		Align128Bytes = 0x00800000,
		Align256Bytes = 0x00900000,
		Align512Bytes = 0x00a00000,
		Align1024Bytes = 0x00b00000,
		Align2048Bytes = 0x00c00000,
		Align4096Bytes = 0x00d00000,
		Align8192Bytes = 0x00e00000,
		ExtendedRelocationOverflow = 0x01000000,
		MemoryDiscardable = 0x02000000,
		MemoryNotCached = 0x04000000,
		MemoryNotPaged = 0x08000000,
		MemoryShared = 0x10000000,
		MemoryExecute = 0x20000000,
		MemoryRead = 0x40000000,
		MemoryWrite = 0x80000000,
	};

	enum struct SectionName_e : u8 {
		text,
		bss,
		rdata,
		data,
		pdata,
		xdata,
		idata,
		edata,
		reloc,
		debug,
		rsrc,
		tls,
		crt,
		cormeta,
	};

}

