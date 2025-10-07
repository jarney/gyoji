#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend::tree {

    // Syntax Terminals
    const JLang::context::TokenID TERMINAL_NAMESPACE = 0;
    const JLang::context::TokenID TERMINAL_USING = 1;
    const JLang::context::TokenID TERMINAL_AS = 2;
    const JLang::context::TokenID TERMINAL_TYPEDEF = 3;
    const JLang::context::TokenID TERMINAL_CLASS = 4;
    const JLang::context::TokenID TERMINAL_PUBLIC = 5;
    const JLang::context::TokenID TERMINAL_ENUM = 6;
    const JLang::context::TokenID TERMINAL_PRIVATE = 7;
    const JLang::context::TokenID TERMINAL_PROTECTED = 8;
    const JLang::context::TokenID TERMINAL_STRUCT = 9;
    const JLang::context::TokenID TERMINAL_UNION = 10;
    
    const JLang::context::TokenID TERMINAL_IF = 11;
    const JLang::context::TokenID TERMINAL_ELSE = 12;
    const JLang::context::TokenID TERMINAL_WHILE = 13;
    const JLang::context::TokenID TERMINAL_FOR = 14;
    const JLang::context::TokenID TERMINAL_SWITCH = 15;
    const JLang::context::TokenID TERMINAL_RETURN = 16;
    const JLang::context::TokenID TERMINAL_BREAK = 17;
    const JLang::context::TokenID TERMINAL_CONTINUE = 18;
    const JLang::context::TokenID TERMINAL_LABEL = 19;
    const JLang::context::TokenID TERMINAL_GOTO = 20;
    
    const JLang::context::TokenID TERMINAL_CASE = 21;
    const JLang::context::TokenID TERMINAL_DEFAULT = 22;
    const JLang::context::TokenID TERMINAL_SIZEOF = 23;
    const JLang::context::TokenID TERMINAL_CAST = 24;
    const JLang::context::TokenID TERMINAL_TYPEOF = 25;
    const JLang::context::TokenID TERMINAL_CONST = 26;
    const JLang::context::TokenID TERMINAL_VOLATILE = 27;
    const JLang::context::TokenID TERMINAL_UNSAFE = 28;
    const JLang::context::TokenID TERMINAL_SEMICOLON = 29;
    const JLang::context::TokenID TERMINAL_PTR_OP = 30;
    
    const JLang::context::TokenID TERMINAL_RIGHT_OP = 31;
    const JLang::context::TokenID TERMINAL_INC_OP = 32;
    const JLang::context::TokenID TERMINAL_DEC_OP = 33;
    const JLang::context::TokenID TERMINAL_LEFT_OP = 34;
    const JLang::context::TokenID TERMINAL_LT_OP = 35;
    const JLang::context::TokenID TERMINAL_GT_OP = 36;
    const JLang::context::TokenID TERMINAL_LE_OP = 37;
    const JLang::context::TokenID TERMINAL_GE_OP = 38;
    const JLang::context::TokenID TERMINAL_EQ_OP = 39;
    const JLang::context::TokenID TERMINAL_NE_OP = 40;
    
    const JLang::context::TokenID TERMINAL_XOR_OP = 41;
    const JLang::context::TokenID TERMINAL_OR_OP = 42;
    const JLang::context::TokenID TERMINAL_MUL_ASSIGN = 43;
    const JLang::context::TokenID TERMINAL_DIV_ASSIGN = 44;
    const JLang::context::TokenID TERMINAL_ADD_ASSIGN = 45;
    const JLang::context::TokenID TERMINAL_SUB_ASSIGN = 46;
    const JLang::context::TokenID TERMINAL_LEFT_ASSIGN = 47;
    const JLang::context::TokenID TERMINAL_RIGHT_ASSIGN = 48;
    const JLang::context::TokenID TERMINAL_AND_ASSIGN = 49;
    const JLang::context::TokenID TERMINAL_XOR_ASSIGN = 50;
    
    const JLang::context::TokenID TERMINAL_OR_ASSIGN = 51;
    const JLang::context::TokenID TERMINAL_PAREN_L = 52;
    const JLang::context::TokenID TERMINAL_PAREN_R = 53;
    const JLang::context::TokenID TERMINAL_BRACKET_L = 54;
    const JLang::context::TokenID TERMINAL_BRACKET_R = 55;
    const JLang::context::TokenID TERMINAL_BRACE_L = 56;
    const JLang::context::TokenID TERMINAL_BRACE_R = 57;
    const JLang::context::TokenID TERMINAL_DOT = 58;
    const JLang::context::TokenID TERMINAL_QUESTIONMARK = 59;
    const JLang::context::TokenID TERMINAL_COLON = 60;
    
    const JLang::context::TokenID TERMINAL_COMMA = 61;
    const JLang::context::TokenID TERMINAL_BANG = 62;
    const JLang::context::TokenID TERMINAL_TILDE = 63;
    const JLang::context::TokenID TERMINAL_ANDPERSAND = 64;
    const JLang::context::TokenID TERMINAL_PIPE = 65;
    const JLang::context::TokenID TERMINAL_PLUS = 66;
    const JLang::context::TokenID TERMINAL_MINUS = 67;
    const JLang::context::TokenID TERMINAL_STAR = 68;
    const JLang::context::TokenID TERMINAL_SLASH = 69;
    const JLang::context::TokenID TERMINAL_PERCENT = 70;
    
    const JLang::context::TokenID TERMINAL_EQUALS = 71;
    const JLang::context::TokenID TERMINAL_NAMESPACE_NAME = 72;
    const JLang::context::TokenID TERMINAL_TYPE_NAME = 73;
    const JLang::context::TokenID TERMINAL_BOOL = 74;
    const JLang::context::TokenID TERMINAL_IDENTIFIER = 75;
    const JLang::context::TokenID TERMINAL_LITERAL_BOOL = 76;
    const JLang::context::TokenID TERMINAL_LITERAL_CHAR = 77;
    const JLang::context::TokenID TERMINAL_LITERAL_STRING = 78;
    const JLang::context::TokenID TERMINAL_LITERAL_NULL = 79;
    const JLang::context::TokenID TERMINAL_LITERAL_FLOAT = 80;
    
    const JLang::context::TokenID TERMINAL_LITERAL_INT = 81;
    const JLang::context::TokenID TERMINAL_YYEOF = 82;
    const JLang::context::TokenID TERMINAL_comment = 83;
    const JLang::context::TokenID TERMINAL_single_line_comment = 84;
    const JLang::context::TokenID TERMINAL_whitespace = 85;
    const JLang::context::TokenID TERMINAL_newline = 86;
    const JLang::context::TokenID TERMINAL_file_metadata = 87;
    
    // Syntax Non-terminals
    const JLang::context::TokenID NONTERMINAL_access_modifier = 200;
    const JLang::context::TokenID NONTERMINAL_access_qualifier = 201;
    const JLang::context::TokenID NONTERMINAL_argument_expression_list = 202;
    const JLang::context::TokenID NONTERMINAL_array_length = 203;
    const JLang::context::TokenID NONTERMINAL_class_argument_list = 204;
    const JLang::context::TokenID NONTERMINAL_class_declaration = 205;
    const JLang::context::TokenID NONTERMINAL_class_decl_start = 206;
    const JLang::context::TokenID NONTERMINAL_class_definition = 207;
    const JLang::context::TokenID NONTERMINAL_class_member_declaration = 208;
    const JLang::context::TokenID NONTERMINAL_class_member_declaration_list = 209;
    const JLang::context::TokenID NONTERMINAL_class_member_declaration_method = 210;
    const JLang::context::TokenID NONTERMINAL_class_member_declaration_variable = 211;
    const JLang::context::TokenID NONTERMINAL_enum_definition = 212;
    const JLang::context::TokenID NONTERMINAL_enum_definition_value = 213;
    const JLang::context::TokenID NONTERMINAL_enum_definition_value_list = 214;
    const JLang::context::TokenID NONTERMINAL_expression = 215;
    const JLang::context::TokenID NONTERMINAL_expression_binary = 216;
    const JLang::context::TokenID NONTERMINAL_expression_cast = 217;
    const JLang::context::TokenID NONTERMINAL_expression_postfix_array_index = 218;
    const JLang::context::TokenID NONTERMINAL_expression_postfix_arrow = 219;
    const JLang::context::TokenID NONTERMINAL_expression_postfix_dot = 220;
    const JLang::context::TokenID NONTERMINAL_expression_postfix_function_call = 221;
    const JLang::context::TokenID NONTERMINAL_expression_postfix_incdec = 222;
    const JLang::context::TokenID NONTERMINAL_expression_primary_identifier = 223;
    const JLang::context::TokenID NONTERMINAL_expression_primary_literal_bool = 224;
    const JLang::context::TokenID NONTERMINAL_expression_primary_literal_char = 225;
    const JLang::context::TokenID NONTERMINAL_expression_primary_literal_float = 226;
    const JLang::context::TokenID NONTERMINAL_expression_primary_literal_int = 227;
    const JLang::context::TokenID NONTERMINAL_expression_primary_literal_null = 228;
    const JLang::context::TokenID NONTERMINAL_expression_primary_literal_string = 229;
    const JLang::context::TokenID NONTERMINAL_expression_primary_nested = 230;
    const JLang::context::TokenID NONTERMINAL_expression_trinary = 231;
    const JLang::context::TokenID NONTERMINAL_expression_unary_prefix = 232;
    const JLang::context::TokenID NONTERMINAL_expression_unary_sizeof_type = 233;
    const JLang::context::TokenID NONTERMINAL_file_statement = 234;
    const JLang::context::TokenID NONTERMINAL_file_statement_function_declaration = 235;
    const JLang::context::TokenID NONTERMINAL_file_statement_global_definition = 236;
    const JLang::context::TokenID NONTERMINAL_file_statement_list = 237;
    const JLang::context::TokenID NONTERMINAL_file_statement_namespace = 238;
    const JLang::context::TokenID NONTERMINAL_file_statement_using = 239;
    const JLang::context::TokenID NONTERMINAL_function_definition_arg = 240;
    const JLang::context::TokenID NONTERMINAL_function_definition_arg_list = 241;
    const JLang::context::TokenID NONTERMINAL_global_initializer = 242;
    const JLang::context::TokenID NONTERMINAL_global_initializer_addressof_expression_primary = 243;
    const JLang::context::TokenID NONTERMINAL_global_initializer_expression_primary = 244;
    const JLang::context::TokenID NONTERMINAL_global_initializer_struct_initializer_list = 245;
    const JLang::context::TokenID NONTERMINAL_namespace_declaration = 246;
    const JLang::context::TokenID NONTERMINAL_scope_body = 247;
    const JLang::context::TokenID NONTERMINAL_statement = 248;
    const JLang::context::TokenID NONTERMINAL_statement_block = 249;
    const JLang::context::TokenID NONTERMINAL_statement_break = 250;
    const JLang::context::TokenID NONTERMINAL_statement_continue = 251;
    const JLang::context::TokenID NONTERMINAL_statement_expression = 252;
    const JLang::context::TokenID NONTERMINAL_statement_for = 253;
    const JLang::context::TokenID NONTERMINAL_statement_goto = 254;
    const JLang::context::TokenID NONTERMINAL_statement_ifelse = 255;
    const JLang::context::TokenID NONTERMINAL_statement_label = 256;
    const JLang::context::TokenID NONTERMINAL_statement_list = 257;
    const JLang::context::TokenID NONTERMINAL_statement_return = 258;
    const JLang::context::TokenID NONTERMINAL_statement_switch = 259;
    const JLang::context::TokenID NONTERMINAL_statement_switch_block = 260;
    const JLang::context::TokenID NONTERMINAL_statement_switch_content = 261;
    const JLang::context::TokenID NONTERMINAL_statement_variable_declaration = 262;
    const JLang::context::TokenID NONTERMINAL_statement_while = 263;
    const JLang::context::TokenID NONTERMINAL_struct_initializer = 264;
    const JLang::context::TokenID NONTERMINAL_struct_initializer_list = 265;
    const JLang::context::TokenID NONTERMINAL_terminal = 266;
    const JLang::context::TokenID NONTERMINAL_translation_unit = 267;
    const JLang::context::TokenID NONTERMINAL_type_definition = 268;
    const JLang::context::TokenID NONTERMINAL_type_name = 269;
    const JLang::context::TokenID NONTERMINAL_type_specifier = 270;
    const JLang::context::TokenID NONTERMINAL_type_specifier_array = 271;
    const JLang::context::TokenID NONTERMINAL_type_specifier_call_args = 272;
    const JLang::context::TokenID NONTERMINAL_type_specifier_function_pointer = 273;
    const JLang::context::TokenID NONTERMINAL_type_specifier_pointer_to = 274;
    const JLang::context::TokenID NONTERMINAL_type_specifier_reference_to = 275;
    const JLang::context::TokenID NONTERMINAL_type_specifier_simple = 276;
    const JLang::context::TokenID NONTERMINAL_type_specifier_template = 277;
    const JLang::context::TokenID NONTERMINAL_unsafe_modifier = 278;
    const JLang::context::TokenID NONTERMINAL_using_as = 279;
    const JLang::context::TokenID NONTERMINAL_initializer_expression = 280;
    
    const JLang::context::TokenID END_OF_TOKENS = 9999;


};

