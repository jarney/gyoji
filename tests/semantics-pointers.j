u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	u32 a;
	u32 *ptr_to_a;
	u32 c;

	a = 10;
	c = 17;
	ptr_to_a = &a;
	*ptr_to_a = 20;
	c = *ptr_to_a;
	c = 19;
	a = 23;
	ptr_to_a = &c;

	print_value(a);
// Wrong argument type, this is an error.
//	print_value(ptr_to_a);
	print_value(*ptr_to_a);

	print_value(c);

	return 0;
}