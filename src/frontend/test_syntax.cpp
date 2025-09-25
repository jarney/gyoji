#include <jlang-frontend.hpp>
#include <jlang-misc/input-source-file.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;

static
JLang::owned<ParseResult>
parse(std::string & path, CompilerContext & compiler_context, std::string base_filename)
{
    std::string filename = path + std::string("/") + base_filename;
    
    int input = open(filename.c_str(), O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", base_filename.c_str());
	return nullptr;
    }
    JLang::misc::InputSourceFile input_source(input);
    JLang::owned<ParseResult> parse_result =
	Parser::parse(compiler_context,
		      input_source
	    );

    close(input);
    
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
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/llvm-decl-var.j");
	ASSERT_FALSE(context.has_errors(), "tests/llvm-decl-var.j had an unexpected syntax error");
	ASSERT_NOT_NULL(parse_result, "Parse of known-good thing should not be null");
	ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
	ASSERT_INT_EQUAL(7, parse_result->get_translation_unit().get_statements().size(), "Wrong number of statements in file");
    }
    
    {
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/syntax-empty.j");
	ASSERT_FALSE(context.has_errors(), "tests/syntax-empty.j had an unexpected syntax error");
	ASSERT_NOT_NULL(parse_result, "Empty file should parse");
	ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
	ASSERT_INT_EQUAL(0, parse_result->get_translation_unit().get_statements().size(), "Empty file should have no statements");
    }
    
    {
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/syntax-invalid-garbage.j");
	ASSERT_TRUE(parse_result->has_errors(), "We expect a syntax error in this file");
	ASSERT_INT_EQUAL(1, parse_result->get_errors().size(), "We should have exactly one error");
	ASSERT_INT_EQUAL(1, parse_result->get_errors().get(0).size(), "That error should have exactly one message");
	ASSERT_INT_EQUAL(4, parse_result->get_errors().get(0).get(0).get_line(), "Error should appear on line 4");
    }
    
    // Check typedefs and associated modifiers.
    {
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/syntax-typedef.j");
	ASSERT_FALSE(context.has_errors(), "tests/syntax-typedef.j had an unexpected syntax error");
	ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
	ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
	ASSERT_INT_EQUAL(4, parse_result->get_translation_unit().get_statements().size(), "This should have 4 typedefs");
	{    
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeDefinition>>(statement_type), "This should be a typedef");
	    
	    const auto & type_definition = std::get<JLang::owned<TypeDefinition>>(statement_type);
	    ASSERT("char", type_definition->get_name(), "We expect that we are defining 'char'");
	    
	    const auto & access_modifier = type_definition->get_access_modifier();
	    ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");
	}
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(1)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeDefinition>>(statement_type), "This should be a typedef");
	    
	    const auto & type_definition = std::get<JLang::owned<TypeDefinition>>(statement_type);
	    ASSERT("public_char", type_definition->get_name(), "We expect that we are defining 'char'");
	    
	    const auto & access_modifier = type_definition->get_access_modifier();
	    ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");
	}
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(2)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeDefinition>>(statement_type), "This should be a typedef");
	    
	    const auto & type_definition = std::get<JLang::owned<TypeDefinition>>(statement_type);
	    ASSERT("protected_char", type_definition->get_name(), "We expect that we are defining 'char'");
	    
	    const auto & access_modifier = type_definition->get_access_modifier();
	    ASSERT_INT_EQUAL(AccessModifier::PROTECTED, access_modifier.get_type(), "We expect this to be protected.");
	}
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(3)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeDefinition>>(statement_type), "This should be a typedef");
	    
	    const auto & type_definition = std::get<JLang::owned<TypeDefinition>>(statement_type);
	    ASSERT("private_char", type_definition->get_name(), "We expect that we are defining 'char'");
	    
	    const auto & access_modifier = type_definition->get_access_modifier();
	    ASSERT_INT_EQUAL(AccessModifier::PRIVATE, access_modifier.get_type(), "We expect this to be public by default");
	}
    }
    {
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/syntax-access-qualifier.j");
	ASSERT_FALSE(context.has_errors(), "tests/syntax-access-qualifier.j had an unexpected syntax error");
	ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
	ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
	ASSERT_INT_EQUAL(2, parse_result->get_translation_unit().get_statements().size(), "This should have 2 global initializations.");
	
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<FileStatementGlobalDefinition>>(statement_type), "This should be a global definition");
	    
	    const auto & global_definition = std::get<JLang::owned<FileStatementGlobalDefinition>>(statement_type);
	    ASSERT("x", global_definition->get_name(), "We expect that we are defining the variable 'x'");
	    
	    const auto & access_modifier = global_definition->get_access_modifier();
	    ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");
	    
	    const auto & type_specifier = global_definition->get_type_specifier();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeSpecifierSimple>>(type_specifier.get_type()), "Expect a simple type.");
	    const auto & type_specifier_simple = std::get<JLang::owned<TypeSpecifierSimple>>(type_specifier.get_type());
	    const auto & access_qualifier = type_specifier_simple->get_access_qualifier();
	    ASSERT_INT_EQUAL(AccessQualifier::CONST, access_qualifier.get_type(), "Expect this to be const qualified");
	    
	    const auto & type_name = type_specifier_simple->get_type_name();
	    ASSERT_FALSE(type_name.is_expression(), "This should be a simple type and not an expression");
	    ASSERT_STR_EQUAL("u32", type_name.get_name(), "This should be just a u32");
	    
	}
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(1)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<FileStatementGlobalDefinition>>(statement_type), "This should be a global definition");
	    
	    const auto & global_definition = std::get<JLang::owned<FileStatementGlobalDefinition>>(statement_type);
	    ASSERT("p", global_definition->get_name(), "We expect that we are defining the variable 'p'");
	    
	    const auto & access_modifier = global_definition->get_access_modifier();
	    ASSERT_INT_EQUAL(AccessModifier::PUBLIC, access_modifier.get_type(), "We expect this to be public by default");
	    
	    const auto & type_specifier = global_definition->get_type_specifier();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeSpecifierSimple>>(type_specifier.get_type()), "Expect a simple type.");
	    const auto & type_specifier_simple = std::get<JLang::owned<TypeSpecifierSimple>>(type_specifier.get_type());
	    const auto & access_qualifier = type_specifier_simple->get_access_qualifier();
	    ASSERT_INT_EQUAL(AccessQualifier::VOLATILE, access_qualifier.get_type(), "Expect this to be const qualified");
	    
	    const auto & type_name = type_specifier_simple->get_type_name();
	    ASSERT_FALSE(type_name.is_expression(), "This should be a simple type and not an expression");
	    ASSERT_STR_EQUAL("f64", type_name.get_name(), "This should be just a f64");
	}
    }
    
    {
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/syntax-pointer.j");
	ASSERT_FALSE(context.has_errors(), "tests/syntax-pointer.j had an unexpected syntax error");
	ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
	ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
	ASSERT_INT_EQUAL(2, parse_result->get_translation_unit().get_statements().size(), "This should have 2 global initializations.");
	
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<FileStatementGlobalDefinition>>(statement_type), "This should be a global definition");
	    
	    const auto & global_definition = std::get<JLang::owned<FileStatementGlobalDefinition>>(statement_type);
	    ASSERT("p", global_definition->get_name(), "We expect that we are defining the variable 'x'");
	    
	    const auto & type_specifier = global_definition->get_type_specifier();
	    
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeSpecifierPointerTo>>(type_specifier.get_type()), "Expect a pointer to another type.");
	    const auto & type_specifier_pointer_to = std::get<JLang::owned<TypeSpecifierPointerTo>>(type_specifier.get_type());
	    
	    const auto & access_qualifier = type_specifier_pointer_to->get_access_qualifier();
	    ASSERT_INT_EQUAL(AccessQualifier::VOLATILE, access_qualifier.get_type(), "Expect this to be const qualified");
	    
	    const auto & type_referred_to = type_specifier_pointer_to->get_type_specifier();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeSpecifierSimple>>(type_referred_to.get_type()), "Expect a simple type we're referring to.");
	    const auto & type_specifier_referred_to_simple = std::get<JLang::owned<TypeSpecifierSimple>>(type_referred_to.get_type());
	    
	    const auto & access_qualifier_referred_to = type_specifier_referred_to_simple->get_access_qualifier();
	    ASSERT_INT_EQUAL(AccessQualifier::CONST, access_qualifier_referred_to.get_type(), "Expect this to be const qualified");
	}
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(1)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<FileStatementGlobalDefinition>>(statement_type), "This should be a global definition");
	    
	    const auto & global_definition = std::get<JLang::owned<FileStatementGlobalDefinition>>(statement_type);
	    ASSERT("q", global_definition->get_name(), "We expect that we are defining the variable 'p'");
	    
	    const auto & type_specifier = global_definition->get_type_specifier();
	    
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeSpecifierReferenceTo>>(type_specifier.get_type()), "Expect a pointer to another type.");
	    const auto & type_specifier_reference_to = std::get<JLang::owned<TypeSpecifierReferenceTo>>(type_specifier.get_type());
	    
	    const auto & access_qualifier = type_specifier_reference_to->get_access_qualifier();
	    ASSERT_INT_EQUAL(AccessQualifier::CONST, access_qualifier.get_type(), "Expect this to be const qualified");
	    
	    const auto & type_referred_to = type_specifier_reference_to->get_type_specifier();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<TypeSpecifierSimple>>(type_referred_to.get_type()), "Expect a simple type we're referring to.");
	    const auto & type_specifier_referred_to_simple = std::get<JLang::owned<TypeSpecifierSimple>>(type_referred_to.get_type());
	    
	    const auto & access_qualifier_referred_to = type_specifier_referred_to_simple->get_access_qualifier();
	    ASSERT_INT_EQUAL(AccessQualifier::VOLATILE, access_qualifier_referred_to.get_type(), "Expect this to be const qualified");
	}
    }
    {
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/syntax-function-declaration.j");
	ASSERT_FALSE(context.has_errors(), "tests/syntax-function-declaration.j had an unexpected syntax error");
	ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
	ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
	ASSERT_INT_EQUAL(2, parse_result->get_translation_unit().get_statements().size(), "This should have a function declaration.");
	
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<FileStatementFunctionDeclaration>>(statement_type), "This should be a function declaration");
	    
	    const auto & function_declaration = std::get<JLang::owned<FileStatementFunctionDeclaration>>(statement_type);
	    const auto & unsafe_modifier = function_declaration->get_unsafe_modifier();
	    ASSERT_FALSE(unsafe_modifier.is_unsafe(), "This one should be safe.");
	    
	    const auto & argument_list = function_declaration->get_arguments();
	    ASSERT_INT_EQUAL(3, argument_list.get_arguments().size(), "Should be 3 arguments");
	}
	
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(1)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<FileStatementFunctionDeclaration>>(statement_type), "This should be a function declaration");
	    
	    const auto & function_declaration = std::get<JLang::owned<FileStatementFunctionDeclaration>>(statement_type);
	    const auto & unsafe_modifier = function_declaration->get_unsafe_modifier();
	    ASSERT_TRUE(unsafe_modifier.is_unsafe(), "This one should be unsafe.");
	    
	    const auto & argument_list = function_declaration->get_arguments();
	    ASSERT_INT_EQUAL(3, argument_list.get_arguments().size(), "Should be 3 arguments");
	}
    }
    {
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/syntax-function-definition.j");
	ASSERT_FALSE(context.has_errors(), "tests/syntax-function-definition.j had an unexpected syntax error");
	ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
	ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
	ASSERT_INT_EQUAL(1, parse_result->get_translation_unit().get_statements().size(), "This should have a function definition.");
	
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<FileStatementFunctionDefinition>>(statement_type), "This should be a function definition");
	    
	    const auto & function_definition = std::get<JLang::owned<FileStatementFunctionDefinition>>(statement_type);
	}
    }
    
    {
	CompilerContext context;
	auto parse_result = parse(path, context, "tests/syntax-function-unsafe-block.j");
	ASSERT_FALSE(context.has_errors(), "tests/syntax-function-unsafe-block.j had an unexpected syntax error");
	ASSERT_NOT_NULL(parse_result, "File should parse correctly.");
	ASSERT_TRUE(parse_result->has_translation_unit(), "We should have a translation unit");
	ASSERT_INT_EQUAL(1, parse_result->get_translation_unit().get_statements().size(), "This should have a function definition.");
	
	{
	    const auto & statement_type = parse_result->get_translation_unit().get_statements().at(0)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<FileStatementFunctionDefinition>>(statement_type), "This should be a function definition");
	    
	    const auto & function_definition = std::get<JLang::owned<FileStatementFunctionDefinition>>(statement_type);
	    const auto & scope_body = function_definition->get_scope_body();
	    ASSERT_INT_EQUAL(2, scope_body.get_statements().get_statements().size(), "There should be 2 statements in this function");
	    
	    const auto & statement_2 = scope_body.get_statements().get_statements().at(1)->get_statement();
	    ASSERT_TRUE(std::holds_alternative<JLang::owned<StatementBlock>>(statement_2), "This should be a block statement.");
	    const auto & statement_block = std::get<JLang::owned<StatementBlock>>(statement_2);
	    const auto & unsafe_block = statement_block->get_scope_body();
	    ASSERT_INT_EQUAL(1, unsafe_block.get_statements().get_statements().size(), "This unsafe block should have a single statement");
	    
	    const auto & unsafe_modifier = statement_block->get_unsafe_modifier();
	    ASSERT_TRUE(unsafe_modifier.is_unsafe(), "This block should be marked unsafe.");
	}
    }
    
    printf("PASSED\n");
    
    return 0;
}
