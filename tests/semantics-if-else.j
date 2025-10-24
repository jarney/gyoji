
u32 write(u32 fd, u8 *buf, u32 size);

u32 preamble_function_call()
{
    write(0,
    "abcXXdef"
    "012345"
    , 5);
    return 0;
}
u32 nineteen();
u32 nineteen_ish();
u32 twenty_one_nofun();
u32 a_a();
u32 a_b();
u32 bar();
u32 printf_first();
u32 printf_second(u8 *f);
u32 unconditional_function_call();


u32 main(u32 argc, u8 **argv)
{
	u8 *str;
	str = "abcd";

        u32 a = 2;
	a = 19u32;
        preamble_function_call();
        if (a == 2u32) {
	   u32 scope_var = 12;
           a_a();
           if (!(a == 19u32)) {
               nineteen();
	       u32 inner_scope_var;
	       if (scope_var == 2222) {
	           printf_first();
	       }
	       else {
	           printf_second("nested");
	       }
	       nineteen_ish();
           }
           else {
               twenty_one_nofun();
           }
           a_b();
        }
	else {
           bar();
        }
	unconditional_function_call();
	return 33u32;
}

