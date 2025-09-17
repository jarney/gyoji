#include <jlang-frontend.hpp>
#include <jlang-frontend/input-source-file.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::errors;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;

static
ParseResult_owned_ptr
parse(std::string & path, std::string base_filename)
{
  std::string filename = path + std::string("/") + base_filename;
  
  FILE *input = fopen(filename.c_str(), "rb");
  if (input == NULL) {
    fprintf(stderr, "Cannot open file %s\n", base_filename.c_str());
    return nullptr;
  }
  NamespaceContext namespace_context;
  Errors errors;
  InputSourceFile input_source(input);
  Parser parser(namespace_context, errors);
  int rc = parser.parse(input_source);
  if (rc != 0) {
    fprintf(stderr, "Syntax error : %s\n", base_filename.c_str());
    return nullptr;
  }
  ParseResult_owned_ptr parse_result = parser.get_parse_result();
  return std::move(parse_result);
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "First argument must be the path of the source directory\n");
    return -1;
  }
  std::string path(argv[1]);

  // Check that we have the right number of statements in this file.
  {
    auto tu = parse(path, "tests/llvm-decl-var.j");
    ASSERT_NOT_NULL(tu, "Parse of known-good thing should not be null");
    ASSERT_INT_EQUAL(tu->get_translation_unit().get_statements().size(), 6, "Wrong number of statements in file");
  }

  {
    auto tu = parse(path, "tests/syntax-empty.j");
    ASSERT_NOT_NULL(tu, "Empty file should parse");
    ASSERT_INT_EQUAL(tu->get_translation_unit().get_statements().size(), 0, "Empty file should have no statements");
  }

  {
    //    printf("Expecting syntax error on line 4\n");
    //    auto tu = tester.parse("tests/syntax-invalid-garbage.j");
    //    ASSERT_NULL(tu, "Garbage file should not parse");
    //    printf("Expectation met\n");
  }
  
  printf("PASSED\n");
  
  return 0;
}
