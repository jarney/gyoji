
void print_u8(u8 val);
void print_u16(u16 val);
void print_u32(u32 val);
void print_u64(u64 val);

void print_i8(i8 val);
void print_i16(i16 val);
void print_i32(i32 val);
void print_i64(i64 val);

u32 main(u32 argc, u8** argv)
{
	u8 vu8;
        vu8 = 1_2_8u8;
	if (vu8 != 0x80u8) {
            return 1;
	}
	print_u8(vu8);

	i8 vi8;
	vi8 = -0x03i8;
	if (vi8 != -3i8) {
            return 1;
	}

	u16 vu16;
	vu16 = 65_535u16;
	if (vu16 != 0xffffu16) {
            return 1;
	}
	vu16 = 0b01101u16;
	print_u16(vu16);

	u32 vu32;
	vu32 = 165_535u32;
	if (vu32 != 165_535u32) {
            return 1;
	}
	print_u32(vu32);

	u64 vu64;
	vu64 = 164_535u64;
	if (vu64 != 164_535u64) {
            return 1;
	}
	print_u64(vu64);


// Unsigned types:
        u8 u8val;
	u8val = 10u8;
	u8val = 0x0a_u8;
	u8val = 0b0100_0100_u8;
	
	u16 u16val;
	u16val = 1024u16;
	u16val = 0x10ffu16;
	u16val = 0o0777u16;
	u16val = 0b1000_0101_u16;

	u32 u32val;
	u32val = 1024u32;
	u32val = 0xffff_ffffu32;
	u32val = 0o0777u32;
	u32val = 0b0100_0000_1100_0011_u32;
	u32val = 1024;

	u64 u64val;
	u64val = 1_024_u64;
	u64val = 0xffff_ffff_ffff_ffff_u64;
	u64val = 0b1100_0000_1100_0011_1100_0000_1100_0011_u64;

// Signed types:
        i8 i8val;
	i8val = 10i8;
	i8val = 0x0a_i8;
	i8val = 0b0100_0100_i8;
	
	i16 i16val;
	i16val = 1024i16;
	i16val = 0x10ffi16;
	i16val = 0b1000_0101_i16;

	i32 i32val;
	i32val = 1024i32;
	i32val = 0x0fff_ffffi32;
	i32val = 0b0100_0000_1100_0011_i32;
	i32val = -1024;
	
	i64 i64val;
	i64val = -1_024_i64;
	i64val = 1_024_i64;
	i64val = -0x0fff_ffff_ffff_ffff_i64;
	i64val = 0x0fff_ffff_ffff_ffff_i64;
	i64val = -0b0100_0000_1100_0011_1100_0000_1100_0011_i64;
	return 0;
}