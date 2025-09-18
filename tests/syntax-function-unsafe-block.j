
u32 foo(u32 *a, u32 b, u8 c)
{
        a = 10;
        unsafe {
            *a = 10;
        }

}
