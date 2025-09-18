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

  NamespaceContext_owned_ptr namespace_context = std::make_unique<NamespaceContext>();
  
  InputSourceFile input_source(input);
  Parser parser(std::move(namespace_context));
  int rc = parser.parse(input_source);
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
    auto parse_result = parse(path, "tests/llvm-decl-var.j");
    ASSERT_NOT_NULL(parse_result, "Parse of known-good thing should not be null");
    ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
    ASSERT_INT_EQUAL(6, parse_result->get_translation_unit().get_statements().size(), "Wrong number of statements in file");
  }

  {
    auto parse_result = parse(path, "tests/syntax-empty.j");
    ASSERT_NOT_NULL(parse_result, "Empty file should parse");
    ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
    ASSERT_INT_EQUAL(0, parse_result->get_translation_unit().get_statements().size(), "Empty file should have no statements");
  }

  {
    auto parse_result = parse(path, "tests/syntax-invalid-garbage.j");
    ASSERT_TRUE(parse_result->has_errors(), "We expect a syntax error in this file");
    ASSERT_INT_EQUAL(1, parse_result->get_errors().size(), "We should have exactly one error");
    ASSERT_INT_EQUAL(1, parse_result->get_errors().get(0).size(), "That error should have exactly one message");
    ASSERT_INT_EQUAL(4, parse_result->get_errors().get(0).get(0).get_line(), "Error should appear on line 4");
  }

  // A single typedef
  {
    auto parse_result = parse(path, "tests/syntax-typedef.j");
    ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
    ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
    ASSERT_INT_EQUAL(1, parse_result->get_translation_unit().get_statements().size(), "This should have one typedef");
    
    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
    ASSERT_TRUE(std::holds_alternative<TypeDefinition_owned_ptr>(statement_type), "This should be a typedef");

    const auto & type_definition = std::get<TypeDefinition_owned_ptr>(statement_type);
    ASSERT("char", type_definition->get_name(), "We expect that we are defining 'char'");

    const auto & access_modifier = type_definition->get_access_modifier();
    ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");
  }
  
  printf("PASSED\n");
  
  return 0;
}
