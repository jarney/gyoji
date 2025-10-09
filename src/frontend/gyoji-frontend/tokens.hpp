#ifndef _GYOJI_INTERNAL
#error "This header is intended to be used internally as a part of the Gyoji front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace Gyoji::frontend::tree {

    // Syntax Terminals
    const Gyoji::context::TokenID TERMINAL_NAMESPACE = 0;
    const Gyoji::context::TokenID TERMINAL_USING = 1;
    const Gyoji::context::TokenID TERMINAL_AS = 2;
    const Gyoji::context::TokenID TERMINAL_TYPEDEF = 3;
    const Gyoji::context::TokenID TERMINAL_CLASS = 4;
    const Gyoji::context::TokenID TERMINAL_PUBLIC = 5;
    const Gyoji::context::TokenID TERMINAL_ENUM = 6;
    const Gyoji::context::TokenID TERMINAL_PRIVATE = 7;
    const Gyoji::context::TokenID TERMINAL_PROTECTED = 8;
    const Gyoji::context::TokenID TERMINAL_STRUCT = 9;
    const Gyoji::context::TokenID TERMINAL_UNION = 10;
    
    const Gyoji::context::TokenID TERMINAL_IF = 11;
    const Gyoji::context::TokenID TERMINAL_ELSE = 12;
    const Gyoji::context::TokenID TERMINAL_WHILE = 13;
    const Gyoji::context::TokenID TERMINAL_FOR = 14;
    const Gyoji::context::TokenID TERMINAL_SWITCH = 15;
    const Gyoji::context::TokenID TERMINAL_RETURN = 16;
    const Gyoji::context::TokenID TERMINAL_BREAK = 17;
    const Gyoji::context::TokenID TERMINAL_CONTINUE = 18;
    const Gyoji::context::TokenID TERMINAL_LABEL = 19;
    const Gyoji::context::TokenID TERMINAL_GOTO = 20;
    
    const Gyoji::context::TokenID TERMINAL_CASE = 21;
    const Gyoji::context::TokenID TERMINAL_DEFAULT = 22;
    const Gyoji::context::TokenID TERMINAL_SIZEOF = 23;
    const Gyoji::context::TokenID TERMINAL_CAST = 24;
    const Gyoji::context::TokenID TERMINAL_TYPEOF = 25;
    const Gyoji::context::TokenID TERMINAL_CONST = 26;
    const Gyoji::context::TokenID TERMINAL_VOLATILE = 27;
    const Gyoji::context::TokenID TERMINAL_UNSAFE = 28;
    const Gyoji::context::TokenID TERMINAL_SEMICOLON = 29;
    const Gyoji::context::TokenID TERMINAL_PTR_OP = 30;
    
    const Gyoji::context::TokenID TERMINAL_RIGHT_OP = 31;
    const Gyoji::context::TokenID TERMINAL_INC_OP = 32;
    const Gyoji::context::TokenID TERMINAL_DEC_OP = 33;
    const Gyoji::context::TokenID TERMINAL_LEFT_OP = 34;
    const Gyoji::context::TokenID TERMINAL_COMPARE_LESS = 35;
    const Gyoji::context::TokenID TERMINAL_COMPARE_GREATER = 36;
    const Gyoji::context::TokenID TERMINAL_COMPARE_LESS_EQUAL = 37;
    const Gyoji::context::TokenID TERMINAL_COMPARE_GREATER_EQUAL = 38;
    const Gyoji::context::TokenID TERMINAL_COMPARE_EQUAL = 39;
    const Gyoji::context::TokenID TERMINAL_COMPARE_NOT_EQUAL = 40;
    
    const Gyoji::context::TokenID TERMINAL_XOR_OP = 41;
    const Gyoji::context::TokenID TERMINAL_OR_OP = 42;
    const Gyoji::context::TokenID TERMINAL_MUL_ASSIGNMENT = 43;
    const Gyoji::context::TokenID TERMINAL_DIV_ASSIGNMENT = 44;
    const Gyoji::context::TokenID TERMINAL_ADD_ASSIGNMENT = 45;
    const Gyoji::context::TokenID TERMINAL_SUB_ASSIGNMENT = 46;
    const Gyoji::context::TokenID TERMINAL_LEFT_ASSIGNMENT = 47;
    const Gyoji::context::TokenID TERMINAL_RIGHT_ASSIGNMENT = 48;
    const Gyoji::context::TokenID TERMINAL_AND_ASSIGNMENT = 49;
    const Gyoji::context::TokenID TERMINAL_XOR_ASSIGNMENT = 50;
    
    const Gyoji::context::TokenID TERMINAL_OR_ASSIGNMENT = 51;
    const Gyoji::context::TokenID TERMINAL_PAREN_L = 52;
    const Gyoji::context::TokenID TERMINAL_PAREN_R = 53;
    const Gyoji::context::TokenID TERMINAL_BRACKET_L = 54;
    const Gyoji::context::TokenID TERMINAL_BRACKET_R = 55;
    const Gyoji::context::TokenID TERMINAL_BRACE_L = 56;
    const Gyoji::context::TokenID TERMINAL_BRACE_R = 57;
    const Gyoji::context::TokenID TERMINAL_DOT = 58;
    const Gyoji::context::TokenID TERMINAL_QUESTIONMARK = 59;
    const Gyoji::context::TokenID TERMINAL_COLON = 60;
    
    const Gyoji::context::TokenID TERMINAL_COMMA = 61;
    const Gyoji::context::TokenID TERMINAL_BANG = 62;
    const Gyoji::context::TokenID TERMINAL_TILDE = 63;
    const Gyoji::context::TokenID TERMINAL_ANDPERSAND = 64;
    const Gyoji::context::TokenID TERMINAL_PIPE = 65;
    const Gyoji::context::TokenID TERMINAL_PLUS = 66;
    const Gyoji::context::TokenID TERMINAL_MINUS = 67;
    const Gyoji::context::TokenID TERMINAL_STAR = 68;
    const Gyoji::context::TokenID TERMINAL_SLASH = 69;
    const Gyoji::context::TokenID TERMINAL_PERCENT = 70;
    
    const Gyoji::context::TokenID TERMINAL_ASSIGNMENT = 71;
    const Gyoji::context::TokenID TERMINAL_NAMESPACE_NAME = 72;
    const Gyoji::context::TokenID TERMINAL_TYPE_NAME = 73;
    const Gyoji::context::TokenID TERMINAL_BOOL = 74;
    const Gyoji::context::TokenID TERMINAL_IDENTIFIER = 75;
    const Gyoji::context::TokenID TERMINAL_LITERAL_BOOL = 76;
    const Gyoji::context::TokenID TERMINAL_LITERAL_CHAR = 77;
    const Gyoji::context::TokenID TERMINAL_LITERAL_STRING = 78;
    const Gyoji::context::TokenID TERMINAL_LITERAL_NULL = 79;
    const Gyoji::context::TokenID TERMINAL_LITERAL_FLOAT = 80;
    
    const Gyoji::context::TokenID TERMINAL_LITERAL_INT = 81;
    const Gyoji::context::TokenID TERMINAL_YYEOF = 82;
    const Gyoji::context::TokenID TERMINAL_comment = 83;
    const Gyoji::context::TokenID TERMINAL_single_line_comment = 84;
    const Gyoji::context::TokenID TERMINAL_whitespace = 85;
    const Gyoji::context::TokenID TERMINAL_newline = 86;
    const Gyoji::context::TokenID TERMINAL_file_metadata = 87;
    
    // Syntax Non-terminals
    const Gyoji::context::TokenID NONTERMINAL_access_modifier = 200;
    const Gyoji::context::TokenID NONTERMINAL_access_qualifier = 201;
    const Gyoji::context::TokenID NONTERMINAL_argument_expression_list = 202;
    const Gyoji::context::TokenID NONTERMINAL_array_length = 203;
    const Gyoji::context::TokenID NONTERMINAL_class_argument_list = 204;
    const Gyoji::context::TokenID NONTERMINAL_class_declaration = 205;
    const Gyoji::context::TokenID NONTERMINAL_class_decl_start = 206;
    const Gyoji::context::TokenID NONTERMINAL_class_definition = 207;
    const Gyoji::context::TokenID NONTERMINAL_class_member_declaration = 208;
    const Gyoji::context::TokenID NONTERMINAL_class_member_declaration_list = 209;
    const Gyoji::context::TokenID NONTERMINAL_class_member_declaration_method = 210;
    const Gyoji::context::TokenID NONTERMINAL_class_member_declaration_variable = 211;
    const Gyoji::context::TokenID NONTERMINAL_enum_definition = 212;
    const Gyoji::context::TokenID NONTERMINAL_enum_definition_value = 213;
    const Gyoji::context::TokenID NONTERMINAL_enum_definition_value_list = 214;
    const Gyoji::context::TokenID NONTERMINAL_expression = 215;
    const Gyoji::context::TokenID NONTERMINAL_expression_binary = 216;
    const Gyoji::context::TokenID NONTERMINAL_expression_cast = 217;
    const Gyoji::context::TokenID NONTERMINAL_expression_postfix_array_index = 218;
    const Gyoji::context::TokenID NONTERMINAL_expression_postfix_arrow = 219;
    const Gyoji::context::TokenID NONTERMINAL_expression_postfix_dot = 220;
    const Gyoji::context::TokenID NONTERMINAL_expression_postfix_function_call = 221;
    const Gyoji::context::TokenID NONTERMINAL_expression_postfix_incdec = 222;
    const Gyoji::context::TokenID NONTERMINAL_expression_primary_identifier = 223;
    const Gyoji::context::TokenID NONTERMINAL_expression_primary_literal_bool = 224;
    const Gyoji::context::TokenID NONTERMINAL_expression_primary_literal_char = 225;
    const Gyoji::context::TokenID NONTERMINAL_expression_primary_literal_float = 226;
    const Gyoji::context::TokenID NONTERMINAL_expression_primary_literal_int = 227;
    const Gyoji::context::TokenID NONTERMINAL_expression_primary_literal_null = 228;
    const Gyoji::context::TokenID NONTERMINAL_expression_primary_literal_string = 229;
    const Gyoji::context::TokenID NONTERMINAL_expression_primary_nested = 230;
    const Gyoji::context::TokenID NONTERMINAL_expression_trinary = 231;
    const Gyoji::context::TokenID NONTERMINAL_expression_unary_prefix = 232;
    const Gyoji::context::TokenID NONTERMINAL_expression_unary_sizeof_type = 233;
    const Gyoji::context::TokenID NONTERMINAL_file_statement = 234;
    const Gyoji::context::TokenID NONTERMINAL_file_statement_function_declaration = 235;
    const Gyoji::context::TokenID NONTERMINAL_file_statement_global_definition = 236;
    const Gyoji::context::TokenID NONTERMINAL_file_statement_list = 237;
    const Gyoji::context::TokenID NONTERMINAL_file_statement_namespace = 238;
    const Gyoji::context::TokenID NONTERMINAL_file_statement_using = 239;
    const Gyoji::context::TokenID NONTERMINAL_function_definition_arg = 240;
    const Gyoji::context::TokenID NONTERMINAL_function_definition_arg_list = 241;
    const Gyoji::context::TokenID NONTERMINAL_global_initializer = 242;
    const Gyoji::context::TokenID NONTERMINAL_global_initializer_addressof_expression_primary = 243;
    const Gyoji::context::TokenID NONTERMINAL_global_initializer_expression_primary = 244;
    const Gyoji::context::TokenID NONTERMINAL_global_initializer_struct_initializer_list = 245;
    const Gyoji::context::TokenID NONTERMINAL_namespace_declaration = 246;
    const Gyoji::context::TokenID NONTERMINAL_scope_body = 247;
    const Gyoji::context::TokenID NONTERMINAL_statement = 248;
    const Gyoji::context::TokenID NONTERMINAL_statement_block = 249;
    const Gyoji::context::TokenID NONTERMINAL_statement_break = 250;
    const Gyoji::context::TokenID NONTERMINAL_statement_continue = 251;
    const Gyoji::context::TokenID NONTERMINAL_statement_expression = 252;
    const Gyoji::context::TokenID NONTERMINAL_statement_for = 253;
    const Gyoji::context::TokenID NONTERMINAL_statement_goto = 254;
    const Gyoji::context::TokenID NONTERMINAL_statement_ifelse = 255;
    const Gyoji::context::TokenID NONTERMINAL_statement_label = 256;
    const Gyoji::context::TokenID NONTERMINAL_statement_list = 257;
    const Gyoji::context::TokenID NONTERMINAL_statement_return = 258;
    const Gyoji::context::TokenID NONTERMINAL_statement_switch = 259;
    const Gyoji::context::TokenID NONTERMINAL_statement_switch_block = 260;
    const Gyoji::context::TokenID NONTERMINAL_statement_switch_content = 261;
    const Gyoji::context::TokenID NONTERMINAL_statement_variable_declaration = 262;
    const Gyoji::context::TokenID NONTERMINAL_statement_while = 263;
    const Gyoji::context::TokenID NONTERMINAL_struct_initializer = 264;
    const Gyoji::context::TokenID NONTERMINAL_struct_initializer_list = 265;
    const Gyoji::context::TokenID NONTERMINAL_terminal = 266;
    const Gyoji::context::TokenID NONTERMINAL_translation_unit = 267;
    const Gyoji::context::TokenID NONTERMINAL_type_definition = 268;
    const Gyoji::context::TokenID NONTERMINAL_type_name = 269;
    const Gyoji::context::TokenID NONTERMINAL_type_specifier = 270;
    const Gyoji::context::TokenID NONTERMINAL_type_specifier_array = 271;
    const Gyoji::context::TokenID NONTERMINAL_type_specifier_call_args = 272;
    const Gyoji::context::TokenID NONTERMINAL_type_specifier_function_pointer = 273;
    const Gyoji::context::TokenID NONTERMINAL_type_specifier_pointer_to = 274;
    const Gyoji::context::TokenID NONTERMINAL_type_specifier_reference_to = 275;
    const Gyoji::context::TokenID NONTERMINAL_type_specifier_simple = 276;
    const Gyoji::context::TokenID NONTERMINAL_type_specifier_template = 277;
    const Gyoji::context::TokenID NONTERMINAL_unsafe_modifier = 278;
    const Gyoji::context::TokenID NONTERMINAL_using_as = 279;
    const Gyoji::context::TokenID NONTERMINAL_initializer_expression = 280;
    
    const Gyoji::context::TokenID END_OF_TOKENS = 9999;


};

