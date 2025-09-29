
unsigned short c_u16_add(unsigned short a, unsigned short b)
{
    unsigned short c;
    c = a + b;
    return c;
}

short c_i16_add(short a, short b)
{
    short c;
    c = a + b;
    return c;
}

unsigned int c_u32_add_widen_a(unsigned short a, unsigned int b)
{
    unsigned int c;
    c = (unsigned int)a + b;
    return c;
}

unsigned int c_u32_add_widen_b(unsigned int a, unsigned short b)
{
    unsigned int c;
    c = a + b;
    return c;
}

unsigned int c_u32_add(unsigned int a, unsigned int b)
{
    unsigned int c;
    c = a + b;
    return c;
}

