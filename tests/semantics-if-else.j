
u32 preamble_function_call();
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
        u32 a = 2;
	a = 19;
        preamble_function_call();
        if (a == 2) {
	   u32 scope_var;
           a_a();
	   if (a == 4) {
	   }
           if (!(a == 19)) {
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
}