#include "namespace.hpp"
#include "test.hpp"

int main(int argc, char **argv)
{
  
  printf("Testing namespace search functionality\n");
  
  namespace_init();

  ASSERT_BOOL(false, namespace_type_exists("string"), "string should not exist before creation");
  ASSERT_BOOL(false, namespace_type_exists("std::string"), "std::string should not exist before creation");
  ASSERT_BOOL(false, namespace_type_exists("::std::string"), "std::string should not exist before creation");
  
  // current namespace is std
  // 
  namespace_begin("std");
  namespace_type_define("string", PROTECTION_PUBLIC);
  
  ASSERT_BOOL(true, namespace_type_exists("string"), "Type should exist in current namespace");
  ASSERT_BOOL(false, namespace_type_exists("bar"), "This type was never defined");
  ASSERT_BOOL(true, namespace_type_exists("std::string"), "String can be accessed directly from root(1)");
  ASSERT_BOOL(true, namespace_type_exists("::std::string"), "String can be accessed directly from root(2)");
  ASSERT_BOOL(true, namespace_type_exists(": : std :: string"), "Should also work when we throw in random whitespace");
  
  namespace_end(); // Namespace is now "" after popping ::std from it.
  
  // Now that we're outside of this namespace,
  // string should no longer be defined
  ASSERT_BOOL(false, namespace_type_exists("string"), "String is no longer in this namespace");
  // but std::string is.
  
  // We can still access this from the root
  ASSERT_BOOL(true, namespace_type_exists("std::string"), "String can be accessed directly from root");
  ASSERT_BOOL(true, namespace_type_exists("::std::string"), "String is in the root namespace");

  namespace_begin("nonstd");
  
  namespace_using("std", "foo");
  namespace_using("std", "");
  ASSERT_BOOL(true, namespace_type_exists("foo::string"), "String can be accessed by namespace alias");
  ASSERT_BOOL(true, namespace_type_exists("string"), "String can be accessed by namespace alias");
  
  namespace_end();
  
  ASSERT_BOOL(true, namespace_type_exists("std::string"), "String can be accessed by namespace alias");
  ASSERT_BOOL(false, namespace_type_exists("foo::string"), "String can be accessed by namespace alias");
  ASSERT_BOOL(false, namespace_type_exists("string"), "String can be accessed by namespace alias");
  
  printf("    PASSED\n");
  return 0;
}
