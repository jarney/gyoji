#include "jformat-tree.hpp"
#include <iostream>
#include <jlang-misc/xml.hpp>

using namespace JLang::misc;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;
using namespace JLang::cmdline;
using namespace JLang::context;

struct TokenName {
    TokenID token;
    const char *name;
};

static TokenName token_names[] = {
    // Syntax Terminals
    {TERMINAL_NAMESPACE, "NAMESPACE"},
    {TERMINAL_USING, "USING"},
    {TERMINAL_AS, "AS"},
    {TERMINAL_TYPEDEF, "TYPEDEF"},
    {TERMINAL_CLASS, "CLASS"},
    {TERMINAL_PUBLIC, "PUBLIC"},
    {TERMINAL_ENUM, "ENUM"},
    {TERMINAL_PRIVATE, "PRIVATE"},
    {TERMINAL_PROTECTED, "PROTECTED"},
    {TERMINAL_STRUCT, "STRUCT"},
    {TERMINAL_UNION, "UNION"},
    
    {TERMINAL_IF, "IF"},
    {TERMINAL_ELSE, "ELSE"},
    {TERMINAL_WHILE, "WHILE"},
    {TERMINAL_FOR, "FOR"},
    {TERMINAL_SWITCH, "SWITCH"},
    {TERMINAL_RETURN, "RETURN"},
    {TERMINAL_BREAK, "BREAK"},
    {TERMINAL_CONTINUE, "CONTINUE"},
    {TERMINAL_LABEL, "LABEL"},
    {TERMINAL_GOTO, "GOTO"},
    
    {TERMINAL_CASE, "CASE"},
    {TERMINAL_DEFAULT, "DEFAULT"},
    {TERMINAL_SIZEOF, "SIZEOF"},
    {TERMINAL_CAST, "CAST"},
    {TERMINAL_TYPEOF, "TYPEOF"},
    {TERMINAL_CONST, "CONST"},
    {TERMINAL_VOLATILE, "VOLATILE"},
    {TERMINAL_UNSAFE, "UNSAFE"},
    {TERMINAL_SEMICOLON, "SEMICOLON"},
    {TERMINAL_PTR_OP, "PTR_OP"},
    
    {TERMINAL_RIGHT_OP, "RIGHT_OP"},
    {TERMINAL_INC_OP, "INC_OP"},
    {TERMINAL_DEC_OP, "DEC_OP"},
    {TERMINAL_LEFT_OP, "LEFT_OP"},
    {TERMINAL_COMPARE_LESS, "COMPARE_LESS"},
    {TERMINAL_COMPARE_GREATER, "COMPARE_GREATER"},
    {TERMINAL_COMPARE_LESS_EQUAL, "COMPARE_LESS_EQUAL"},
    {TERMINAL_COMPARE_GREATER_EQUAL, "COMPARE_GREATER_EQUAL"},
    {TERMINAL_COMPARE_EQUAL, "COMPARE_EQUAL"},
    {TERMINAL_COMPARE_NOT_EQUAL, "COMPARE_NOT_EQUAL"},
    
    {TERMINAL_XOR_OP, "XOR_OP"},
    {TERMINAL_OR_OP, "OR_OP"},
    {TERMINAL_MUL_ASSIGNMENT, "MUL_ASSIGNMENT"},
    {TERMINAL_DIV_ASSIGNMENT, "DIV_ASSIGNMENT"},
    {TERMINAL_ADD_ASSIGNMENT, "ADD_ASSIGNMENT"},
    {TERMINAL_SUB_ASSIGNMENT, "SUB_ASSIGNMENT"},
    {TERMINAL_LEFT_ASSIGNMENT, "LEFT_ASSIGNMENT"},
    {TERMINAL_RIGHT_ASSIGNMENT, "RIGHT_ASSIGNMENT"},
    {TERMINAL_AND_ASSIGNMENT, "AND_ASSIGNMENT"},
    {TERMINAL_XOR_ASSIGNMENT, "XOR_ASSIGNMENT"},
    
    {TERMINAL_OR_ASSIGNMENT, "OR_ASSIGNMENT"},
    {TERMINAL_PAREN_L, "PAREN_L"},
    {TERMINAL_PAREN_R, "PAREN_R"},
    {TERMINAL_BRACKET_L, "BRACKET_L"},
    {TERMINAL_BRACKET_R, "BRACKET_R"},
    {TERMINAL_BRACE_L, "BRACE_L"},
    {TERMINAL_BRACE_R, "BRACE_R"},
    {TERMINAL_DOT, "DOT"},
    {TERMINAL_QUESTIONMARK, "QUESTIONMARK"},
    {TERMINAL_COLON, "COLON"},
    {TERMINAL_COMMA, "COMMA"},
    
    {TERMINAL_BANG, "BANG"},
    {TERMINAL_TILDE, "TILDE"},
    {TERMINAL_ANDPERSAND, "ANDPERSAND"},
    {TERMINAL_PIPE, "PIPE"},
    {TERMINAL_PLUS, "PLUS"},
    {TERMINAL_MINUS, "MINUS"},
    {TERMINAL_STAR, "STAR"},
    {TERMINAL_SLASH, "SLASH"},
    {TERMINAL_PERCENT, "PERCENT"},
    
    {TERMINAL_ASSIGNMENT, "ASSIGNMENT"},
    {TERMINAL_NAMESPACE_NAME, "NAMESPACE_NAME"},
    {TERMINAL_TYPE_NAME, "TYPE_NAME"},
    {TERMINAL_BOOL, "BOOL"},
    {TERMINAL_IDENTIFIER, "IDENTIFIER"},
    {TERMINAL_LITERAL_BOOL, "LITERAL_BOOL"},
    {TERMINAL_LITERAL_CHAR, "LITERAL_CHAR"},
    {TERMINAL_LITERAL_STRING, "LITERAL_STRING"},
    {TERMINAL_LITERAL_NULL, "LITERAL_NULL"},
    {TERMINAL_LITERAL_FLOAT, "LITERAL_FLOAT"},
    
    {TERMINAL_LITERAL_INT, "LITERAL_INT"},
    {TERMINAL_YYEOF, "YYEOF"},
    {TERMINAL_comment, "comment"},
    {TERMINAL_single_line_comment, "single_line_comment"},
    {TERMINAL_whitespace, "whitespace"},
    {TERMINAL_newline, "newline"},
    {TERMINAL_file_metadata, "file_metadata"},
    
    // Syntax Non-terminals
    {NONTERMINAL_access_modifier, "access_modifier"},
    {NONTERMINAL_access_qualifier, "access_qualifier"},
    {NONTERMINAL_argument_expression_list, "argument_expression_list"},
    {NONTERMINAL_array_length, "array_length"},
    {NONTERMINAL_class_argument_list, "class_argument_list"},
    {NONTERMINAL_class_declaration, "class_declaration"},
    {NONTERMINAL_class_decl_start, "class_decl_start"},
    {NONTERMINAL_class_definition, "class_definition"},
    {NONTERMINAL_class_member_declaration, "class_member_declaration"},
    {NONTERMINAL_class_member_declaration_list, "class_member_declaration_list"},

    {NONTERMINAL_class_member_declaration_method, "class_member_declaration_method"},
    {NONTERMINAL_class_member_declaration_variable, "class_member_declaration_variable"},
    {NONTERMINAL_enum_definition, "enum_definition"},
    {NONTERMINAL_enum_definition_value, "enum_definition_value"},
    {NONTERMINAL_enum_definition_value_list, "enum_definition_value_list"},
    {NONTERMINAL_expression, "expression"},
    {NONTERMINAL_expression_binary, "expression_binary"},
    {NONTERMINAL_expression_cast, "expression_cast"},
    {NONTERMINAL_expression_postfix_array_index, "expression_postfix_array_index"},
    {NONTERMINAL_expression_postfix_arrow, "expression_postfix_arrow"},
    {NONTERMINAL_expression_postfix_dot, "expression_postfix_dot"},
    {NONTERMINAL_expression_postfix_function_call, "expression_postfix_function_call"},
    {NONTERMINAL_expression_postfix_incdec, "expression_postfix_incdec"},
    {NONTERMINAL_expression_primary_identifier, "expression_primary_identifier"},
    {NONTERMINAL_expression_primary_literal_bool, "expression_primary_literal_bool"},
    {NONTERMINAL_expression_primary_literal_char, "expression_primary_literal_char"},
    {NONTERMINAL_expression_primary_literal_float, "expression_primary_literal_float"},
    {NONTERMINAL_expression_primary_literal_int, "expression_primary_literal_int"},
    {NONTERMINAL_expression_primary_literal_null, "expression_primary_literal_null"},
    {NONTERMINAL_expression_primary_literal_string, "expression_primary_literal_string"},
    {NONTERMINAL_expression_primary_nested, "expression_primary_nested"},
    {NONTERMINAL_expression_trinary, "expression_trinary"},
    {NONTERMINAL_expression_unary_prefix, "expression_unary_prefix"},
    {NONTERMINAL_expression_unary_sizeof_type, "expression_unary_sizeof_type"},
    {NONTERMINAL_file_statement, "file_statement"},
    {NONTERMINAL_file_statement_function_declaration, "file_statement_function_declaration"},
    {NONTERMINAL_file_statement_global_definition, "file_statement_global_definition"},
    {NONTERMINAL_file_statement_list, "file_statement_list"},
    {NONTERMINAL_file_statement_namespace, "file_statement_namespace"},
    {NONTERMINAL_file_statement_using, "file_statement_using"},
    {NONTERMINAL_function_definition_arg, "function_definition_arg"},
    {NONTERMINAL_function_definition_arg_list, "function_definition_arg_list"},
    {NONTERMINAL_global_initializer, "global_initializer"},
    {NONTERMINAL_global_initializer_addressof_expression_primary, "global_initializer_addressof_expression_primary"},
    {NONTERMINAL_global_initializer_expression_primary, "global_initializer_expression_primary"},
    {NONTERMINAL_global_initializer_struct_initializer_list, "global_initializer_struct_initializer_list"},
    {NONTERMINAL_initializer_expression, "initializer_expression"},
    {NONTERMINAL_namespace_declaration, "namespace_declaration"},
    {NONTERMINAL_scope_body, "scope_body"},
    {NONTERMINAL_statement, "statement"},
    {NONTERMINAL_statement_block, "statement_block"},
    {NONTERMINAL_statement_break, "statement_break"},
    {NONTERMINAL_statement_continue, "statement_continue"},
    {NONTERMINAL_statement_expression, "statement_expression"},
    {NONTERMINAL_statement_for, "statement_for"},
    {NONTERMINAL_statement_goto, "statement_goto"},
    {NONTERMINAL_statement_ifelse, "statement_ifelse"},
    {NONTERMINAL_statement_label, "statement_label"},
    {NONTERMINAL_statement_list, "statement_list"},
    {NONTERMINAL_statement_return, "statement_return"},
    {NONTERMINAL_statement_switch, "statement_switch"},
    {NONTERMINAL_statement_switch_block, "statement_switch_block"},
    {NONTERMINAL_statement_switch_content, "statement_switch_content"},
    {NONTERMINAL_statement_variable_declaration, "statement_variable_declaration"},
    {NONTERMINAL_statement_while, "statement_while"},
    {NONTERMINAL_struct_initializer, "struct_initializer"},
    {NONTERMINAL_struct_initializer_list, "struct_initializer_list"},
    {NONTERMINAL_terminal, "terminal"},
    {NONTERMINAL_translation_unit, "translation_unit"},
    {NONTERMINAL_type_definition, "type_definition"},
    {NONTERMINAL_type_name, "type_name"},
    {NONTERMINAL_type_specifier, "type_specifier"},
    {NONTERMINAL_type_specifier_array, "type_specifier_array"},
    {NONTERMINAL_type_specifier_call_args, "type_specifier_call_args"},
    {NONTERMINAL_type_specifier_function_pointer, "type_specifier_function_pointer"},
    {NONTERMINAL_type_specifier_pointer_to, "type_specifier_pointer_to"},
    {NONTERMINAL_type_specifier_reference_to, "type_specifier_reference_to"},
    {NONTERMINAL_type_specifier_simple, "type_specifier_simple"},
    {NONTERMINAL_type_specifier_template, "type_specifier_template"},
    {NONTERMINAL_unsafe_modifier, "unsafe_modifier"},
    {NONTERMINAL_using_as, "using_as"},

    {END_OF_TOKENS, "END_OF_TOKENS"}
};

JFormatTree::JFormatTree()
    : indent(0)
{
    size_t idx = 0;
    while (true) {
	const TokenName & tn = token_names[idx];
	if (tn.token == END_OF_TOKENS) {
	    break;
	}
	token_map.insert(std::pair(tn.token, std::string(tn.name)));
	idx++;
    };
}
JFormatTree::~JFormatTree()
{}

void JFormatTree::print_indent(void)
{
    for (int i = 0; i < 2*indent; i++) {
	printf(" ");
    }
}

void JFormatTree::print_comment_multi_line(const TerminalNonSyntax & node)
{
    print_indent();
    printf("<comment-multi-line>\n");
    indent++;
    print_indent();
    printf("%s\n", xml_to_cdata(node.get_data()).c_str());
    indent--;
    print_indent();
    printf("</comment-multi-line>\n");
}

void JFormatTree::print_comment_single_line(const TerminalNonSyntax & node)
{
    print_indent();
    printf("<comment-single-line>\n");
    indent++;
    print_indent();
    printf("%s\n", xml_to_cdata(node.get_data()).c_str());
    indent--;
    print_indent();
    printf("</comment-single-line>\n");
}

void JFormatTree::print_whitespace(const TerminalNonSyntax & node)
{
    print_indent();
    printf("<whitespace>%s</whitespace>\n", xml_escape_whitespace(node.get_data()).c_str());
}

void JFormatTree::print_file_metadata(const TerminalNonSyntax & node)
{
    printf("<metadata>%s</metadata>", xml_to_cdata(node.get_data()).c_str());
}


void JFormatTree::print_non_syntax(const TerminalNonSyntax & node)
{
    switch (node.get_type()) {
    case TerminalNonSyntax::Type::EXTRA_COMMENT_MULTI_LINE:
	print_comment_multi_line(node);
	break;
    case TerminalNonSyntax::Type::EXTRA_COMMENT_SINGLE_LINE:
	print_comment_single_line(node);
	break;
    case TerminalNonSyntax::Type::EXTRA_WHITESPACE:
	print_whitespace(node);
	break;
    case TerminalNonSyntax::Type::EXTRA_FILE_METADATA:
	print_file_metadata(node);
	break;
    }
}

std::string
JFormatTree::get_token_name(TokenID token) const
{
    const auto & it = token_map.find(token);
    if (it == token_map.end()) {
	return std::string("Unknown");
    }
    return it->second;
}


int JFormatTree::process(const SyntaxNode & node)
{
    print_indent();
    std::string token_type = get_token_name(node.get_type());
    printf("<node type='%s'", xml_escape_attribute(token_type).c_str());
    if (node.has_data<Terminal>()) {
	const Terminal & terminal = node.get_data<Terminal>();
	if (terminal.get_source_ref().get_line() > 0) {
	    printf(" line='%ld' column='%ld'", terminal.get_source_ref().get_line(), terminal.get_source_ref().get_column());
	}
	if (terminal.get_value().length() != 0) {
	    if (terminal.get_type() == TERMINAL_IDENTIFIER ||
		terminal.get_type() == TERMINAL_TYPE_NAME ||
		terminal.get_type() == TERMINAL_NAMESPACE_NAME
		) {
		printf(" value='%s' fq='%s'",
		       xml_escape_attribute(terminal.get_value()).c_str(),
		       xml_escape_attribute(terminal.get_fully_qualified_name()).c_str()
		    );
	    }
	    else {
		printf(" value='%s'", xml_escape_attribute(terminal.get_value()).c_str());
	    }
	}
    }
    
    bool has_non_syntax_children = false;
    if (node.has_data<Terminal>()) {
	const Terminal & maybe_terminal = node.get_data<Terminal>();
	has_non_syntax_children = maybe_terminal.non_syntax.size() != 0;
    }
    
    if (node.get_children().size() == 0 && !has_non_syntax_children) {
	printf("/>\n");
    }
    else {
	printf(">\n");
	indent++;
	if (node.has_data<Terminal>()) {
	    const Terminal & terminal = node.get_data<Terminal>();
	    for (const auto &non_syntax : terminal.non_syntax) {
		print_non_syntax(*non_syntax);
	    }
	}
	for (auto child : node.get_children()) {
	    process(child);
	}
	indent--;
	print_indent();
	printf("</node>\n");
    }
    return 0;
}
