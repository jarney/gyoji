#pragma once

#include <string>
#include <vector>

namespace JLang::misc {
    
    std::vector<std::string> string_split(const std::string &str, const std::string &delimiter);

    bool endswith(const std::string & s, const std::string & suffix);
    bool contains(const std::string & s, const std::string &v);
    std::string join(const std::vector<std::string> & list, std::string delimiter);
    std::string join_nonempty(const std::vector<std::string> & list, std::string delimiter);
    std::string join_nonempty(const std::string &a, const std::string & b, std::string delimiter);
    
    std::string string_remove_nonidentifier(const std::string & str);
    
    std::string string_replace_start(std::string str, const std::string from, const std::string to);
    
};
