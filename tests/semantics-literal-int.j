
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

	return 0;
}