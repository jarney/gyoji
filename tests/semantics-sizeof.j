
u32 main(u32 argc, u8** argv)
{
    u64 size;
    size = sizeof(u8);
    if (size != 1u64) {
        return 1;
    }
    
    size = sizeof(u16);
    if (size != 2u64) {
        return 1;
    }
    
    size = sizeof(u32);
    if (size != 4u64) {
        return 1;
    }

    size = sizeof(u64);
    if (size != 8u64) {
        return 1;
    }
    
    return 0;
}
