u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	u32 a;

	a = 0;

	{
	    u32 b;
        label foo1:
    	    b = 10;
	    goto end;
	}

    goto foo1;

label end:
    
    return 0;
}
