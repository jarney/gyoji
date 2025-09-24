

u32 main(u32 argc, u8 **argv)
{
        u32 a = 2;
        preamble_function_call();
        if (a == 2) {
	   u32 scope_var;
           a_a();
           if (a == 19) {
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