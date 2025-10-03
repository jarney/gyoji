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
    if (found_pos != std::string::npos && found_pos == s.size() - suffix.size()) {
	return true;
    }
    return false;
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

// TODO: These need to be implemented and
// must guarantee that they are reversible
// in all cases.

bool JLang::misc::string_c_escape(std::string & escaped_string, const std::string & unescaped_string, bool is_char)
{
    // Take an un-escaped string and insert the \n, \r, \e
    // escapes exactly as if it were a string literal expressed
    // in C.  Choose exactly one set of escapes as the 'core'
    // standard set from C, but no more and no less.  This should
    // be the least common denominator for escapes.
    for (char c : unescaped_string) {
	switch (c) {
	case 0x07:
	    escaped_string.push_back('\\');
	    escaped_string.push_back('a');
	    break;
	case 0x08:
	    escaped_string.push_back('\\');
	    escaped_string.push_back('b');
	    break;
	case 0x1b:
	    escaped_string.push_back('\\');
	    escaped_string.push_back('e');
	    break;
	case 0x0c:
	    escaped_string.push_back('\\');
	    escaped_string.push_back('f');
	    break;
	case 0x0a:
	    escaped_string.push_back('\\');
	    escaped_string.push_back('n');
	    break;
	case 0x0d:
	    escaped_string.push_back('\\');
	    escaped_string.push_back('r');
	    break;
	case 0x09:
	    escaped_string.push_back('\\');
	    escaped_string.push_back('t');
	    break;
	case 0x27:
	    if (is_char) {
		escaped_string.push_back('\\');
		escaped_string.push_back('\'');
	    }
	    else {
		escaped_string.push_back('\'');
	    }
	    break;
	case 0x22:
	    if (is_char) {
		escaped_string.push_back('\"');
	    }
	    else {
		escaped_string.push_back('\\');
		escaped_string.push_back('\"');
	    }
	    break;
	case 0x5c:
	    escaped_string.push_back('\\');
	    escaped_string.push_back('\\');
	    break;
	default:
	    escaped_string.push_back(c);
	    break;
	}
    }
    return true;
}

bool
JLang::misc::string_c_unescape(std::string & unescaped_string, size_t & location, const std::string & escaped_string, bool is_char)
{
    // Take the 'traditional' C escape sequences
    // and turn them into their 'traditional' counterparts.
#define NORMAL 0
#define IN_ESCAPE 1

    location = 0;
    
    int state = NORMAL;
    for (char c : escaped_string) {
	switch (state) {
	case NORMAL:
	    if (c == '\\') {
		state = IN_ESCAPE;
	    }
	    else {
		unescaped_string.push_back(c);
	    }
	    break;
	case IN_ESCAPE:
	    switch (c) {
	    case '\\':
		unescaped_string.push_back(c);
		state = NORMAL;
		break;
	    case 'a':
		unescaped_string.push_back(0x07);
		state = NORMAL;
		break;
	    case 'b':
		unescaped_string.push_back(0x08);
		state = NORMAL;
		break;
	    case 'e':
		unescaped_string.push_back(0x1b);
		state = NORMAL;
		break;
	    case 'f':
		unescaped_string.push_back(0x0c);
		state = NORMAL;
		break;
	    case 'n':
		unescaped_string.push_back(0x0a);
		state = NORMAL;
		break;
	    case 'r':
		unescaped_string.push_back(0x0d);
		state = NORMAL;
		break;
	    case 't':
		unescaped_string.push_back(0x09);
		state = NORMAL;
		break;
	    case '\'':
		if (is_char) {
		    unescaped_string.push_back(0x27);
		    state = NORMAL;
		}
		else {
		    return false;
		}
		break;
	    case '\"':
		if (is_char) {
		    return false;
		}
		else {
		    unescaped_string.push_back(0x22);
		    state = NORMAL;
		}
		break;
	    default:
		// This is not a valid escape sequence.
		return false;
		
	    }
	}
	location++;
    }
	
    return true;
}

