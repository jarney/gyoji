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
#pragma once

namespace Gyoji::misc {

    /**
     * @brief Input Source used by lexer
     *
     * @details
     * This interface is used to provide the lexer/parser with a method
     * of reading the input data.  This can be implemented as a
     * file reader to read data from a file and can also be implemented
     * as a string buffer reader to read from memory.
     */
    class InputSource {
    public:
	/**
	 * @brief Constructor just provides an interface
	 *
	 * @details
	 * This class has no concrete implementation, but is just an
	 * interface that other classes may provide implementations for.
	 */
	InputSource();
	
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~InputSource();
	
	/**
	 * Sub-classes must implement this method to provide input to
	 * the parser.
	 * @param buf This is the buffer provided by the parser.  This method
	 *            should place the result of the read into the buffer provided.
	 * @param result This is the number of bytes actually read by the input source.
	 * @param max_size This is the maximum number of bytes to be read
	 *                 from the input source.  Note that result must
	 *                 always be less than or equal to max_size in order
	 *                 to ensure that the buffer is not overflowed.
	 */
	virtual void read(char *buf, int &result, int max_size) = 0;
    };
  
};
