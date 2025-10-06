u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	u32 a;

	a = 0;
	print_value(a);
	print_value(a++);
	print_value(a--);
//	print_value(a);
//	print_value(++a);
//	print_value(--a);
	print_value(a);

	return 0;
}