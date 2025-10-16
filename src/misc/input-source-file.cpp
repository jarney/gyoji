/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
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
