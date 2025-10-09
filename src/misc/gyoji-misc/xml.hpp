#pragma once

#include <string>

namespace Gyoji::misc {
    
    // Hacks because we don't have a 'proper' xml library.
    std::string xml_to_cdata(const std::string & str);
    std::string xml_escape_attribute(const std::string & str);
    std::string xml_escape_whitespace(const std::string & str);
    
};
