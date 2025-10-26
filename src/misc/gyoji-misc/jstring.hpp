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
#include <vector>

namespace Gyoji::misc {
    
    std::vector<std::string> string_split(const std::string &str, const std::string &delimiter);

    std::string pad_string(size_t length, std::string padder);
    std::string wrap_text(size_t max_width, std::string input);    
    std::string indent_text(size_t indent, std::string input);

    bool startswith(const std::string & s, const std::string & prefix);
    bool endswith(const std::string & s, const std::string & suffix);
    std::string join(const std::vector<std::string> & list, std::string delimiter);
    std::string join_nonempty(const std::string &a, const std::string & b, std::string delimiter);
    
    std::string string_remove(const std::string & str, const std::string & remove);
    
    std::string string_replace_start(std::string str, const std::string from, const std::string to);

    bool string_c_escape(std::string & escaped_string, const std::string & unescaped_string, bool is_char);

    /**
     * This function decodes the C escape sequences
     * and places the decoded result in 'unescaped_string'.
     * If there is an unknown or invalid escape sequence, the
     * function returns false and the 'location' is updated
     * with the location of the failed or unknown escape sequence.
     */
    bool string_c_unescape(std::string & unescaped_string, size_t & location, const std::string & escaped_string, bool is_char);
    
    
};
