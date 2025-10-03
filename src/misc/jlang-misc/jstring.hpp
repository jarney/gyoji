#pragma once

#include <string>
#include <vector>

namespace JLang::misc {
    
    std::vector<std::string> string_split(const std::string &str, const std::string &delimiter);

    bool startswith(const std::string & s, const std::string & prefix);
    bool endswith(const std::string & s, const std::string & suffix);
    std::string join(const std::vector<std::string> & list, std::string delimiter);
    std::string join_nonempty(const std::string &a, const std::string & b, std::string delimiter);
    
    std::string string_remove(const std::string & str, const std::string & remove);
    
    std::string string_replace_start(std::string str, const std::string from, const std::string to);

    bool string_c_escape(std::string & escaped_string, const std::string & unescaped_string);

    /**
     * This function decodes the C escape sequences
     * and places the decoded result in 'unescaped_string'.
     * If there is an unknown or invalid escape sequence, the
     * function returns false and the 'location' is updated
     * with the location of the failed or unknown escape sequence.
     */
    bool string_c_unescape(std::string & unescaped_string, size_t & location, const std::string & escaped_string);
    
    
};
