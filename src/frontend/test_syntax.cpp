#include <jlang-frontend.hpp>
#include <jlang-frontend/input-source-file.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::errors;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;

static
::JLang::owned<ParseResult>
parse(std::string & path, std::string base_filename)
{
  std::string filename = path + std::string("/") + base_filename;
  
  FILE *input = fopen(filename.c_str(), "rb");
  if (input == NULL) {
    fprintf(stderr, "Cannot open file %s\n", base_filename.c_str());
    return nullptr;
  }

  ::JLang::owned<NamespaceContext> namespace_context = std::make_unique<NamespaceContext>();
  
  InputSourceFile input_source(input);
  ::JLang::owned<ParseResult> parse_result =
      Parser::parse(std::move(namespace_context),
                   input_source
                   );

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

  // Check typedefs and associated modifiers.
  {
    auto parse_result = parse(path, "tests/syntax-typedef.j");
    ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
    ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
    ASSERT_INT_EQUAL(4, parse_result->get_translation_unit().get_statements().size(), "This should have 4 typedefs");
    {    
      const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeDefinition>>(statement_type), "This should be a typedef");
      
      const auto & type_definition = std::get<::JLang::owned<TypeDefinition>>(statement_type);
      ASSERT("char", type_definition->get_name(), "We expect that we are defining 'char'");
      
      const auto & access_modifier = type_definition->get_access_modifier();
      ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");
    }
    {
      const auto & statement_type = parse_result->get_translation_unit().get_statements().at(1)->get_statement();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeDefinition>>(statement_type), "This should be a typedef");
      
      const auto & type_definition = std::get<::JLang::owned<TypeDefinition>>(statement_type);
      ASSERT("public_char", type_definition->get_name(), "We expect that we are defining 'char'");
      
      const auto & access_modifier = type_definition->get_access_modifier();
      ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");
    }
    {
      const auto & statement_type = parse_result->get_translation_unit().get_statements().at(2)->get_statement();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeDefinition>>(statement_type), "This should be a typedef");

      const auto & type_definition = std::get<::JLang::owned<TypeDefinition>>(statement_type);
      ASSERT("protected_char", type_definition->get_name(), "We expect that we are defining 'char'");
      
      const auto & access_modifier = type_definition->get_access_modifier();
      ASSERT_INT_EQUAL(AccessModifier::PROTECTED, access_modifier.get_type(), "We expect this to be protected.");
    }
    {
      const auto & statement_type = parse_result->get_translation_unit().get_statements().at(3)->get_statement();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeDefinition>>(statement_type), "This should be a typedef");
      
      const auto & type_definition = std::get<::JLang::owned<TypeDefinition>>(statement_type);
      ASSERT("private_char", type_definition->get_name(), "We expect that we are defining 'char'");
      
      const auto & access_modifier = type_definition->get_access_modifier();
      ASSERT_INT_EQUAL(AccessModifier::PRIVATE, access_modifier.get_type(), "We expect this to be public by default");
    }
  }
  {
    auto parse_result = parse(path, "tests/syntax-access-qualifier.j");
    ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
    ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
    ASSERT_INT_EQUAL(2, parse_result->get_translation_unit().get_statements().size(), "This should have 2 global initializations.");

    {
      const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<FileStatementGlobalDefinition>>(statement_type), "This should be a global definition");

      const auto & global_definition = std::get<::JLang::owned<FileStatementGlobalDefinition>>(statement_type);
      ASSERT("x", global_definition->get_name(), "We expect that we are defining the variable 'x'");

      const auto & access_modifier = global_definition->get_access_modifier();
      ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");

      const auto & type_specifier = global_definition->get_type_specifier();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeSpecifierSimple>>(type_specifier.get_type()), "Expect a simple type.");
      const auto & type_specifier_simple = std::get<::JLang::owned<TypeSpecifierSimple>>(type_specifier.get_type());
      const auto & access_qualifier = type_specifier_simple->get_access_qualifier();
      ASSERT_INT_EQUAL(AccessQualifier::CONST, access_qualifier.get_type(), "Expect this to be const qualified");

    }
    {
      const auto & statement_type = parse_result->get_translation_unit().get_statements().at(1)->get_statement();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<FileStatementGlobalDefinition>>(statement_type), "This should be a global definition");

      const auto & global_definition = std::get<::JLang::owned<FileStatementGlobalDefinition>>(statement_type);
      ASSERT("p", global_definition->get_name(), "We expect that we are defining the variable 'p'");

      const auto & access_modifier = global_definition->get_access_modifier();
      ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");

      const auto & type_specifier = global_definition->get_type_specifier();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeSpecifierSimple>>(type_specifier.get_type()), "Expect a simple type.");
      const auto & type_specifier_simple = std::get<::JLang::owned<TypeSpecifierSimple>>(type_specifier.get_type());
      const auto & access_qualifier = type_specifier_simple->get_access_qualifier();
      ASSERT_INT_EQUAL(AccessQualifier::VOLATILE, access_qualifier.get_type(), "Expect this to be const qualified");

    }
  }

  {
    auto parse_result = parse(path, "tests/syntax-pointer.j");
    ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
    ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
    ASSERT_INT_EQUAL(2, parse_result->get_translation_unit().get_statements().size(), "This should have 2 global initializations.");

    {
      const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<FileStatementGlobalDefinition>>(statement_type), "This should be a global definition");

      const auto & global_definition = std::get<::JLang::owned<FileStatementGlobalDefinition>>(statement_type);
      ASSERT("p", global_definition->get_name(), "We expect that we are defining the variable 'x'");

      const auto & type_specifier = global_definition->get_type_specifier();

      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeSpecifierPointerTo>>(type_specifier.get_type()), "Expect a pointer to another type.");
      const auto & type_specifier_pointer_to = std::get<::JLang::owned<TypeSpecifierPointerTo>>(type_specifier.get_type());

      const auto & access_qualifier = type_specifier_pointer_to->get_access_qualifier();
      ASSERT_INT_EQUAL(AccessQualifier::VOLATILE, access_qualifier.get_type(), "Expect this to be const qualified");
      
      const auto & type_referred_to = type_specifier_pointer_to->get_type_specifier();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeSpecifierSimple>>(type_referred_to.get_type()), "Expect a simple type we're referring to.");
      const auto & type_specifier_referred_to_simple = std::get<::JLang::owned<TypeSpecifierSimple>>(type_referred_to.get_type());

      const auto & access_qualifier_referred_to = type_specifier_referred_to_simple->get_access_qualifier();
      ASSERT_INT_EQUAL(AccessQualifier::CONST, access_qualifier_referred_to.get_type(), "Expect this to be const qualified");
    }
    {
      const auto & statement_type = parse_result->get_translation_unit().get_statements().at(1)->get_statement();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<FileStatementGlobalDefinition>>(statement_type), "This should be a global definition");

      const auto & global_definition = std::get<::JLang::owned<FileStatementGlobalDefinition>>(statement_type);
      ASSERT("q", global_definition->get_name(), "We expect that we are defining the variable 'p'");

      const auto & type_specifier = global_definition->get_type_specifier();

      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeSpecifierReferenceTo>>(type_specifier.get_type()), "Expect a pointer to another type.");
      const auto & type_specifier_reference_to = std::get<::JLang::owned<TypeSpecifierReferenceTo>>(type_specifier.get_type());
      
      const auto & access_qualifier = type_specifier_reference_to->get_access_qualifier();
      ASSERT_INT_EQUAL(AccessQualifier::CONST, access_qualifier.get_type(), "Expect this to be const qualified");
      
      const auto & type_referred_to = type_specifier_reference_to->get_type_specifier();
      ASSERT_TRUE(std::holds_alternative<::JLang::owned<TypeSpecifierSimple>>(type_referred_to.get_type()), "Expect a simple type we're referring to.");
      const auto & type_specifier_referred_to_simple = std::get<::JLang::owned<TypeSpecifierSimple>>(type_referred_to.get_type());

      const auto & access_qualifier_referred_to = type_specifier_referred_to_simple->get_access_qualifier();
      ASSERT_INT_EQUAL(AccessQualifier::VOLATILE, access_qualifier_referred_to.get_type(), "Expect this to be const qualified");
    }
  }

  
  printf("PASSED\n");
  
  return 0;
}
