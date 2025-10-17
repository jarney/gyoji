

u32 print_value(u32 number);

u32 main(u32 argc, u8 **argv)
{
	u32 a = 20;

	switch (a) {
	       case 10: {
	           print_value(a);
		   return 4;
	       }
	       case 20: {
	       	    print_value(99u32);
		    return 2;
	       }
	       default: {
	       	       print_value(19);
//		       return 9;
               }
        }
	a = 19;
	return 9;
}


void othermain()
{
	u32 var1;
	u32 var2;

	{
	u32 x = 10;
//            return;
	}

	var1 = 2;
	var2 = var1;
}