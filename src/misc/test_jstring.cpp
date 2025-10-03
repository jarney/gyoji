#include <jlang-misc/jstring.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::misc;

int main(int argc, char **argv)
{
    printf("Testing string manipulations\n");
    
    ASSERT(std::string("std::string"), string_replace_start("foo::string", "foo", "std"), "Replace first part of string");
    ASSERT(std::string("std::string"), string_replace_start("std::string", "string", "foo"), "Replacing the middle doesn't work");
    ASSERT(std::string("std::string"), string_replace_start("string", "", "std::"), "Replace empty string at front");

    {
	std::vector<std::string> strs;
	strs.push_back("one");
	strs.push_back("two");
	ASSERT(std::string("one::two"), join(strs, "::"), "Join should use delimiter");
    }
    {
	std::vector<std::string> strs;
	strs.push_back("one");
	ASSERT(std::string("one"), join(strs, "::"), "If there is just one, it should be no delmimiter");
    }

    std::string int_string("178u64");
    ASSERT_TRUE(endswith(int_string, "u64"), "Expect that we end with the u64 suffix");

    // Processing C string literal escapes.
    {
	std::string c_string("abc\\n\\r\\tx");
	std::string raw_string;
	ASSERT_TRUE(string_c_unescape(raw_string, c_string), "Correctly escape this string");
	std::string c_string_result;
	ASSERT_TRUE(string_c_escape(c_string_result, raw_string), "Correctly unescape this string");
	ASSERT(c_string, c_string_result, "Escape and unescape should yield the same result\n");
    }
    
    {
	std::string c_string("Chanko: ちゃんこ鍋");
	std::string raw_string;
	ASSERT_TRUE(string_c_unescape(raw_string, c_string), "Correctly escape this string");
	std::string c_string_result;
	ASSERT_TRUE(string_c_escape(c_string_result, raw_string), "Correctly unescape this string");
	ASSERT(c_string, c_string_result, "Escape and unescape should yield the same result\n");
    }

    printf("    PASSED\n");
}
