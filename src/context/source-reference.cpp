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

#include <gyoji-context.hpp>

using namespace Gyoji::context;



SourceReference::SourceReference(
    const std::string & _filename,
    size_t _line,
    size_t _column,
    size_t _length
    )
    : filename(_filename)
    , line(_line)
    , column(_column)
    , length(_length)
{}
SourceReference::SourceReference(const SourceReference & _other)
    : filename(_other.filename)
    , line(_other.line)
    , column(_other.column)
    , length(_other.length)
{}
SourceReference::~SourceReference()
{}
const std::string &
SourceReference::get_filename() const
{ return filename; }

size_t SourceReference::get_line() const
{ return line; }

size_t SourceReference::get_column() const
{ return column; }

size_t SourceReference::get_length() const
{ return length; }

