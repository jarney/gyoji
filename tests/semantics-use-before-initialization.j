
u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	u32 b;
	if (argc == 0) {
		b = 10;
	}
	else {
	     b = 12;
	}
	print_value(b);

	u32 a;
	while (argc == 0) {
	      a = 10;
	}
	print_value(a);

	return 0;
}