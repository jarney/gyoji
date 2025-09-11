#include <jlang-misc/jstring.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::misc;

int main(int argc, char **argv)
{
  printf("Testing string manipulations\n");
  
  ASSERT(std::string("std::string"), string_replace_start("foo::string", "foo", "std"), "Replace first part of string");
  ASSERT(std::string("std::string"), string_replace_start("std::string", "string", "foo"), "Replacing the middle doesn't work");
  ASSERT(std::string("std::string"), string_replace_start("string", "", "std::"), "Replace empty string at front");
  
  printf("    PASSED\n");
}
