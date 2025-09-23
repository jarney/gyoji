

u32 main(u32 argc, u8 **argv)
{
        u32 a = 2;
        preamble_function_call();
        if (a == 2) {
           a_a();
           if (a == 19) {
               nineteen();
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