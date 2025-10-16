#include <stdio.h>

#define u32 unsigned int
#define u8 unsigned char

u32 print_value(u32 number)
{
    fprintf(stderr, "Output is %u\n", number);
    return 0;
}

//u32 preamble_function_call()
//{
//    fprintf(stderr, "preamble_function_call\n");
//}
u32 nineteen()
{
    fprintf(stderr, "nineteen\n");
    return 0;
}
u32 nineteen_ish()
{
    fprintf(stderr, "nineteen-ish\n");
    return 0;
}
u32 twenty_one_nofun()
{
    fprintf(stderr, "twenty-one-nofun\n");
    return 0;
}
u32 a_a()
{
    fprintf(stderr, "a-a\n");
    return 0;
}
u32 a_b()
{
    fprintf(stderr, "a-b\n");
    return 0;
}
u32 bar()
{
    fprintf(stderr, "bar\n");
    return 0;
}
u32 printf_first()
{
    fprintf(stderr, "print-first\n");
    return 0;
}
u32 printf_second(u8 *f)
{
    fprintf(stderr, "second\n");
    return 0;
}
u32 unconditional_function_call()
{
    fprintf(stderr, "unconditional-function-call\n");
    return 0;
}
