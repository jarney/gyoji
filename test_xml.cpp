#include <string>
#include <stdio.h>
#include <vector>
#include "xml.hpp"

#define ASSERT(actual, expect, message) \
  {                                                                   \
    std::string actual_str = actual;                                  \
    std::string expect_str = expect;                                  \
      if (expect_str != actual_str) {                                 \
        fprintf(stderr, "Assertion failed: %s\n", message);           \
        fprintf(stderr, "Expected : %s\n", actual_str.c_str());       \
        fprintf(stderr, "Actual   : %s\n", expect_str.c_str());       \
        exit(1);                                                      \
      }                                                               \
  }

int main(int argc, char **argv)
{
  printf("Testing xml cdata conversions\n");
  
  ASSERT(std::string("<![CDATA[This is a ]]]]><![CDATA[> new string]]>"), xml_to_cdata("This is a ]]> new string"), "Two strings with cdata in the middle");
  ASSERT(std::string("<![CDATA[This is a ]]]]><![CDATA[> new string]]]]><![CDATA[>]]>"), xml_to_cdata("This is a ]]> new string]]>"), "Two strings with cdata at the very end");
  ASSERT(std::string("<![CDATA[This is a test]]>"), xml_to_cdata("This is a test"), "Single string with no cdata delimiters");

  ASSERT(std::string("this&quot; is a &quot; good &quot; thing"), xml_escape_attribute("this\" is a \" good \" thing"), "Testing escape of quote character");
  
  printf("    PASSED\n");
}
