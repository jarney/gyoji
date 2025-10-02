#include <algorithm>
#include <jlang-misc/jstring.hpp>

using namespace JLang::misc;

std::vector<std::string> JLang::misc::string_split(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> ret;
    size_t pos = 0;
    while (true) {
	size_t newpos = str.find(delimiter, pos);
	if (newpos == std::string::npos) {
	    break;
	}
	ret.push_back(str.substr(pos, newpos-pos));
	pos = newpos + delimiter.size();
    }
    if (pos == std::string::npos) {
    }
    else {
	ret.push_back(str.substr(pos));
    }
    return ret;
}

std::string
JLang::misc::join_nonempty(const std::string &a, const std::string & b, const std::string delimiter)
{
    std::string ret;
    if (a.size() == 0) {
	return b;
    }
    else {
	return a + delimiter + b;
    }
}

std::string JLang::misc::join_nonempty(const std::vector<std::string> & list, std::string delimiter)
{
    std::string ret;

    bool first = true;
    for (const auto & s : list) {
	if (s.size() == 0) continue;
	if (!first) {
	    ret = ret + delimiter;
	}
	first = false;
	ret = ret + s;
    }
    
    return ret;
}

std::string JLang::misc::join(const std::vector<std::string> & list, std::string delimiter)
{
    std::string ret;

    bool first = true;
    for (const auto & s : list) {
	if (!first) {
	    ret = ret + delimiter;
	}
	first = false;
	ret = ret + s;
    }
    
    return ret;
}

std::string JLang::misc::string_remove_nonidentifier(const std::string & str)
{
    std::string newStr;
    newStr.assign(str);  
    
    for(size_t i = 0; i < str.length(); i++){
	char c = str.at(i);
	if (isalnum(c) || c == ':' || c == '_') {
	    continue;
	}
	newStr.erase(std::remove(newStr.begin(), newStr.end(), c), newStr.end());
    }
    
    return newStr; 
}

bool JLang::misc::startswith(const std::string & s, const std::string & prefix)
{
    size_t found_pos = s.find(prefix);
    if (found_pos == 0) {
	return true;
    }
    return false;
}
bool JLang::misc::endswith(const std::string & s, const std::string & suffix)
{
    size_t found_pos = s.find(suffix);
    if (found_pos == s.size() - suffix.size()) {
	return true;
    }
    return false;
}
bool JLang::misc::contains(const std::string & s, const std::string &v)
{
    size_t found_pos = s.find(v);
    if (found_pos == std::string::npos) {
	return false;
    }
    return true;
}

std::string
JLang::misc::string_remove(const std::string & str, const std::string & remove)
{
    std::string ret(str);
    while (true) {
	size_t start_pos = ret.find(remove);
	// String is not found at all.
	if (start_pos == std::string::npos) {
	    break;
	}

	ret.erase(start_pos, remove.length());
    }
    return ret;
    
}

std::string
JLang::misc::string_replace_start(std::string str, const std::string from, const std::string to)
{
    std::string ret(str);
    size_t start_pos = str.find(from);
    
    // String is not found at all.
    if (start_pos == std::string::npos) {
	return ret;
    }
    // If the string is not found at the start, so do nothing.
    if (start_pos != 0) {
	return ret;
    }
    
    // Replace the string.
    ret.replace(start_pos, from.length(), to);
    return ret;
}
