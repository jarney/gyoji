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
using namespace gyoji::std::string;

Container::~Container()
{}

Contained &
Container::get_contained()
{
    unsafe {
        Contained& ret = &contained;
        return ret;
    }
}
