

u32 print_value(u32 number);

u32 main(u32 argc, u8 **argv)
{
	u32 a = 20;

	switch (a) {
	       case 10: {
	           print_value(a);
	       }
	       case 20: {
	       	    print_value(99u32);
	       }
	       default: {
	       	       print_value(19);
	       }
	}
	return 0;
}
