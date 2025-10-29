#include <strcpy.gh>

// C stdlib functions/system calls.
u8* malloc(u32 n);
void free(u8 * ptr);
u32 write(u32 fd, u8* buf, u32 size);


void
gyoji::std::string::strcpy(u8 *dst, u8* src)
{
    unsafe {
        while (*src != 0u8) {
            *dst = *src;
            dst++;
            src++;
        }
    }
}

u32
gyoji::std::string::strlen(u8 *str)
{
    u32 len = 0;
    unsafe {
        while (*str != 0u8) {
            str++;
            len++;
        }
    }
    return len;
}


        
using namespace gyoji::std::string;

u32 main(u32 argc, u8 **argv)
{
            u8 *source = "abc\n";
            u8 *dest = malloc(5u32);
            strcpy(dest, source);

            unsafe {
                dest = dest + 2;
                *dest = 'k';
                dest = dest - 2;
            }
            
            write(1, source, strlen(source));
            write(1, dest, strlen(dest));

            free(dest);
            return 0;
}
           
