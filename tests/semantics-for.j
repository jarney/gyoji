u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	for (u32 a = 0; a < 20; a += 2) {
	    print_value(a);
        }
	return 0;
}
