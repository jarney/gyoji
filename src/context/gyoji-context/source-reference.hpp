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

#include <string>

namespace Gyoji::context {
    /**
     * @brief References a location in the source-file
     *
     * @details
     * This is a reference to a point in the source
     * file and is used as a way to mark portions
     * of the code so that the error handler has
     * a way to bring the source code to the context
     * of an error message.
     */
    class SourceReference {
    public:
	SourceReference(
	    const std::string & _filename,
	    size_t _line,
	    size_t _column,
	    size_t _length
	    );
	SourceReference(const SourceReference & _other);
	/**
	 * Move along, nothing to see here.
	 */
	~SourceReference();
	const std::string & get_filename() const;
	size_t get_line() const;
	size_t get_column() const;
	size_t get_length() const;
    private:
	const std::string & filename;
	size_t line;
	size_t column;
	size_t length;
    };
};
