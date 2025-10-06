u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	u32 a;

	a = 0;

label top:
	if (a < 10) {
	    u32 b;
	    b = 1;
            print_value(a);
            a = a + b;
	    goto top;
	}
	print_value(a);

	return 0;
}