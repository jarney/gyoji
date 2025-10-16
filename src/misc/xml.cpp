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
#include <string>
#include <gyoji-misc/xml.hpp>
#include <gyoji-misc/jstring.hpp>

using namespace Gyoji::misc;

std::string Gyoji::misc::xml_to_cdata(const std::string & str)
{
    std::string output;
    std::vector<std::string> split = string_split(str, "]]>");
    for (size_t i = 0; i < split.size(); i++) {
	bool has_prev = i > 0;
	bool has_next = i < split.size()-1;
	std::string next(has_next ? "]]" : "");
	std::string prev(has_prev ? ">" : "");
	output = output + "<![CDATA[" + prev + split[i].c_str() + next + "]]>";
    }
    return output;
}

static std::string replace_all(const std::string& source, const std::string& from, const std::string & to)
{
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations
    
    std::string::size_type lastPos = 0;
    std::string::size_type findPos;
    
    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }
    
    // Care for the rest after last occurrence
    newString += source.substr(lastPos);
    
    return newString;
}

std::string Gyoji::misc::xml_escape_attribute(const std::string & str)
{
    return replace_all(replace_all(str, "\'", "&apos;"), "\"", "&quot;");
}

std::string Gyoji::misc::xml_escape_whitespace(const std::string & str)
{
    return replace_all(
	replace_all(
	    str,
	    "\n", "&#xA;"
	    ),
	"\r",
	"&#xD;"
	);
}
