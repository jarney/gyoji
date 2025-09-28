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
    
    printf("    PASSED\n");
}
