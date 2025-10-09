#include <gyoji-misc/input-source-file.hpp>
#include <errno.h>
#include <unistd.h>

using namespace Gyoji::misc;

InputSourceFile::InputSourceFile(int _fd)
    : fd(_fd)
{}
InputSourceFile::~InputSourceFile()
{}

void InputSourceFile::read(char *buf, int &result, int max_size)
{
    errno = 0;
    result = (int) ::read(fd, buf, (size_t) max_size);
    if (result == -1) {
	if( errno != EINTR) {
	    fprintf(stderr, "Fatal error reading input buffer %d\n", errno);
	}
    }
    errno = 0;
}
