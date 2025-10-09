u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	u32 a;

    goto nowhere;
    
	a = 0;

	{
	    u32 b;
        label foo1:
    	    b = 10;
	    goto end;
	}

    goto foo1;

    u32 c;
label end:
    
    return 0;
}
