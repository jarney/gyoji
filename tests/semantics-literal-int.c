#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long i64;

void print_u8(u8 val)
{
    fprintf(stderr, "u8 value is %d\n", val);
}

void print_u16(u16 val)
{
    fprintf(stderr, "u16 value is %d\n", val);
}

void print_u32(u32 val)
{
    fprintf(stderr, "u32 value is %d\n", val);
}

void print_u64(u64 val)
{
    fprintf(stderr, "u64 value is %ld\n", val);
}

void print_i8(i8 val)
{
    fprintf(stderr, "i8 value is %d\n", val);
}
void print_i16(i16 val)
{
    fprintf(stderr, "i16 value is %d\n", val);
}

void print_i32(i32 val)
{
    fprintf(stderr, "i32 value is %d\n", val);
}
void print_i64(i64 val)
{
    fprintf(stderr, "i64 value is %ld\n", val);
}
