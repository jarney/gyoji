u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
        u32 a = 0u32;

        while (a < 10) {
              if ((a % 2) == 0) {
                    print_value(a);
              }
              else {
                    print_value(a+17u32);
              }
              if (a == 9) {
                     print_value(0xffff);
                     break;
              }
              a++;
              if (a == 8) {
                  if (1 == 1) {
                         print_value(a);
                         print_value(0xeeee);
                         continue;
                  }
              }
	      print_value(0u32);
        }

        return 0;
}