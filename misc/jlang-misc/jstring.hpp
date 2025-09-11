#pragma once

#include <string>
#include <vector>

namespace JLang::misc {

  std::vector<std::string> string_split(const std::string &str, const std::string &delimiter);
  
  std::string string_remove_nonidentifier(const std::string & str);
  
  std::string string_replace_start(std::string str, const std::string from, const std::string to);

};
