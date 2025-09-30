#include <stdio.h>

#define u32 unsigned int
#define u8 unsigned char

//u32 preamble_function_call()
//{
//    fprintf(stderr, "preamble_function_call\n");
//}
u32 nineteen()
{
    fprintf(stderr, "nineteen\n");
}
u32 nineteen_ish()
{
    fprintf(stderr, "nineteen-ish\n");
}
u32 twenty_one_nofun()
{
    fprintf(stderr, "twenty-one-nofun\n");
}
u32 a_a()
{
    fprintf(stderr, "a-a\n");
}
u32 a_b()
{
    fprintf(stderr, "a-b\n");
}
u32 bar()
{
    fprintf(stderr, "bar\n");
}
u32 printf_first()
{
    fprintf(stderr, "print-first\n");
}
u32 printf_second(u8 *f)
{
    fprintf(stderr, "second\n");
}
u32 unconditional_function_call()
{
    fprintf(stderr, "unconditional-function-call\n");
}
