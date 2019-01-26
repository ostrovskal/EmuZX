#pragma once

#pragma pack(push, 1)
struct HEAD1_Z80 {
	ssh_w AF, BC, HL, PC, SP;
	ssh_b I, R;
	ssh_b STATE1;
	ssh_w DE;
	ssh_w BC_, DE_, HL_, AF_;
	ssh_w IY, IX;
	ssh_b IFF1, IFF2;
	ssh_b STATE2;
};

struct HEAD2_Z80 {
	// длина HEAD2 + HEAD3
	ssh_w length;
	// Program Counter
	ssh_w PC;
	// Hardware Mode
	ssh_b hardMode;
	/* Hardware States:
	SamRam:		bitwise state of 74LS259 chip;
	128k:		last OUT to 7FFDh(bits 0..3 contain the current switched RAM bank)
	Timex:		last OUT to port F4h */
	ssh_b hardState;
	/* Interface 1 Rom paged ? or Timex output state
	non-Timex:	value 255 for yes, otherwise no
	Timex:		last OUT to port FFh*/
	ssh_b interfaceState;
	/*	R register emulation?	bit 0:	1=yes, 0=no
	LDIR emulation?			bit 1:	1=yes, 0=no
	hardware modification?	bit 7:	1=hardware modification, see below */
	ssh_b emulateFlags;
	// Last OUT to FFFDh (sound chip reg. number)
	ssh_b sndChipRegNumber;
	// Contents of the sound chip register
	ssh_b sndRegs[16];
};
struct HEAD3_Z80 {
	// Low T-State counter
	ssh_w lowTSTATE;
	// High T-State counter
	ssh_b highTSTATE;
	ssh_b reserved;
	// M.G.T: Rom paged ? value 255 for yes, otherwise no
	ssh_b mgtRomPaged;
	// Multiface Rom paged?	value 255 for yes, otherwise no; 
	//						should always be zero as the Multiface ROM is not working without the Multiface RAM, that is not stored in the snapshot
	ssh_b multifaceRomPaged;
	// memory 0000h..1FFFh	value 255 for ROM-area, 0 for RAM
	ssh_b mem0000_1FFF;
	// memory 2000h..3FFFh	value 255 for ROM-area, 0 for RAM
	ssh_b mem2000_3FFF;
	// 5x keyboard mappings	for user-defined joysticks (default=103h, 203h, 403h, 803h, 1003h)
	ssh_b keyMaps[10];
	// 5x ascii values	correspond to keybd.mappings (default=31h, 32h, 33h, 34h, 35h), MSB - byte is always zero.
	ssh_b asciiVals[10];
	// M.G.T. type 0 = Disciple + Epson, 1 = Disciple + HP, 16 = Plus D
	ssh_b mgtType;
	// Disciple inhibitor button	value 255 for in (enabled), 0 for out (disabled)
	ssh_b inhibitorButton;
	// Disciple inhibitor flag		value 255 for Rom not pageable, 0 for Rom pageable
	ssh_b inhibitorFlag;
	// last out to port 1FFDh	control register for Scorpion 256k: Bit no. 4 controls whether RAM pages 0..7 (bit = 0) or 8..15 (bit = 1) are used.
	ssh_b port1FFFD;
	/* DATA BLOCKS:
	2	length of datablock: value 65535 means "uncompressed" may be compressed.
	uncompressed block size=16384 bytes
	1	page number of RAM block	48k:		4=(8000h..BFFFh),
	5=(C000h..FFFFh),
	8=(4000h..7FFFh)
	128k
	Pentagon:	3..10 = Ram pages 0 to 7
	SamRam:		4=(8000h..BFFFh),
	5=(C000h..FFFFh),
	6=(8000h..BFFFh shadow),
	7=(C000h..FFFFh shadow),
	8=(4000h..7FFFh)
	Scorpion:	3..18 = Ram pages 0..15 or
	0..15 = Ram pages 0..15 (!)
	blocklength	compressed data	compression method see version 1 files, but end marker is missing.

	HARDWARE MODES:
	byte value:		version 2:									version 3:
	без модификации		с модификацией			без модификации			с модификацией
	0				Spectrum 48k		Spectrum 16k			Spectrum 48k			Spectrum 16k
	1				Spectrum 48k + IF1	Spectrum 16k+IF1		Spectrum 48k + IF1		Spectrum 16k + IF1
	2												SamRam
	3				Spectrum 128k		Spectrum +2				Spectrum 48k + M.G.T.	Spectrum +2 + M.G.T.
	4				Spectrum 128k+IF1	Spectrum +2 + IF1		Spectrum 128k			Spectrum +2
	5				Spectrum 128k+IF1	Spectrum +2 + IF1		Spectrum 128k + IF1		Spectrum +2 + IF1
	6				Spectrum 128k+M.G.T.Spectrum +2 + M.G.T.	Spectrum 128k + M.G.T.	Spectrum +2 + M.G.T.
	7				Spectrum +3			Spectrum +2A			Spectrum +3				Spectrum +2A
	8				Spectrum +3			Spectrum +2A			Spectrum +3				Spectrum +2A
	9												Pentagon 128k
	10												Scorpion 256k
	11												Didaktik+Kompakt
	12												Spectrum +2
	13												Spectrum +2A
	14												Timex TC 2048
	15												Timex TC 2068
	128												Timex TS 2068
	*/
};
#pragma pack(pop)

bool loadZ80(const StringZX& path);
bool saveZ80(const StringZX& path);
