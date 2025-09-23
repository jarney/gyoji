#include <string>
#include <jlang-misc/xml.hpp>
#include <jlang-misc/jstring.hpp>

using namespace JLang::misc;

std::string JLang::misc::xml_to_cdata(const std::string & str)
{
    std::string output;
    std::vector<std::string> split = string_split(str, "]]>");
    for (int i = 0; i < split.size(); i++) {
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

std::string JLang::misc::xml_escape_attribute(const std::string & str)
{
    return replace_all(replace_all(str, "\'", "&apos;"), "\"", "&quot;");
}

std::string JLang::misc::xml_escape_whitespace(const std::string & str)
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
