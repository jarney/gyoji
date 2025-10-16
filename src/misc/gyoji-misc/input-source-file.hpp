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
#include <gyoji-misc/input-source.hpp>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

namespace Gyoji::misc {

    /**
     * @brief This is an input source for files.
     *
     * @details
     * This is an implementation of an input source based
     * on a stdio FILE* descriptor, reading the data
     * from the given file.
     */
    class InputSourceFile : public InputSource {
    public:
	/**
	 * @brief Create input source by file descriptor.
	 *
	 * @details
	 * Constructs an input source reader for
	 * the given file.  Note that the caller is
	 * responsible for opening and closing the file.
	 * This reader merely uses the file provided to gather
	 * the data, but assumes no responsibility for its
	 * overall lifetime.
	 */
	InputSourceFile(int _fd);
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~InputSourceFile();
	/**
	 * @brief Method to read input from the file.
	 *
	 * @details
	 * This provides the parser with data from the given
	 * file using 'fread' to read from the file and
	 * provide data directly to the buffer.  The 'result'
	 * represents the number of bytes actually read.
	 */
	void read(char *buf, int &result, int max_size);

    private:
	int fd;
    };

};
