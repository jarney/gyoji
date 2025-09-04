#include <string>
#include <vector>
#include "xml.hpp"

static std::vector<std::string> string_split(const std::string &str, const std::string &delimiter)
{
  std::vector<std::string> ret;
  size_t pos = 0;
  while (true) {
    size_t newpos = str.find(delimiter, pos);
    if (newpos == std::string::npos) {
      break;
    }
    ret.push_back(str.substr(pos, newpos-pos));
    pos = newpos + 3;
  }
  if (pos == std::string::npos) {
  }
  else {
    ret.push_back(str.substr(pos));
  }
  return ret;
}

std::string xml_to_cdata(const std::string & str)
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

std::string xml_escape_attribute(const std::string & str)
{
  return replace_all(str, "\"", "&quot;");
}

std::string xml_escape_whitespace(const std::string & str)
{
  return replace_all(replace_all(str,
                                 "\n", "&#xA;"),
                     "\r", "&#xD;");
}
