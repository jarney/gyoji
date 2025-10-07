u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
	u32 a;

	a = 0;

	while (a < 10) {
	      if ((a % 2) == 0) {
	      	      print_value(a);
	      }
	      else {
	      	      print_value(17u32);
	      }
	      if (a == 6) {
	              print_value(0xffff);
	      	      break;
	      }
	      a++;
	      if (a == 5) {
		  if (1 == 1) {
	          print_value(a);
	          print_value(0xeeee);
    		      break;
		  }
	      }
	}

	return 0;
}