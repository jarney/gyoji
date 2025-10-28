#include <strcpy.gh>

void
gyoji::std::string::strcpy(u8 *dst, u8* src)
{
    unsafe {
        while (*dst != 0u8) {
            *dst = *src;
            // We still need pointer math
            // to make this work, otherwise
            // it won't advance the pointer.
            //dst++;
            //src++;
        }
    }
}
