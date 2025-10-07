u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
        u32 a = 4u32;

	for (a = 0; a < 20; a += 2) {
	    print_value(a);
        }
}
