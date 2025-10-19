u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	u32[3] a;

	a[0] = 19;
	a[1] = 20;

	u32 *b;
	b = &a[0];
	unsafe {
		*b = 23;
	}

	print_value(a[0]);
	print_value(a[1]);
	unsafe {
		return *b;
	}
}