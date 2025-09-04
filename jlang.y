%{
#include <iostream>
#include <string>
#include <cmath>
#include <memory>
#include "target/jlang.l.hpp"
#include "jsyntax.hpp"
#include "ast.hpp"
%}
 
%require "3.7.4"
%language "C++"
%defines "target/jlang.y.hpp"
%output "target/jlang.y.cpp"
 
%define api.parser.class {Parser}
%define api.namespace {calc}
%define api.value.type variant
%param {yyscan_t scanner}
 
%code provides
{
#define DEBUG_NONTERMINALS 0
#if DEBUG_NONTERMINALS
#define PRINT_NONTERMINALS(s) printf("%s\n", s->type_name.c_str())
#else
#define PRINT_NONTERMINALS(s) (0)
#endif
  
#define YY_DECL                                                         \
        int yylex(calc::Parser::semantic_type *yylval, yyscan_t yyscanner)
    YY_DECL;
}

%token INVALID_INPUT
%token <ASTNode::ptr> YYEOF
%token <ASTNode::ptr> IDENTIFIER

%token <ASTNode::ptr> NAMESPACE
%token <ASTNode::ptr> USING
%token <ASTNode::ptr> TYPEDEF
%token <ASTNode::ptr> STRUCT
%token <ASTNode::ptr> UNION
%token <ASTNode::ptr> SIZEOF
%token <ASTNode::ptr> TYPEOF
%token <ASTNode::ptr> CAST
%token <ASTNode::ptr> UNSAFE
%token <ASTNode::ptr> VAR
%token <ASTNode::ptr> FUNCTION
%token <ASTNode::ptr> CONST
%token <ASTNode::ptr> VOLATILE

%token <ASTNode::ptr> PUBLIC
%token <ASTNode::ptr> PRIVATE
%token <ASTNode::ptr> PROTECTED

 /* Logical Operations */
%token <ASTNode::ptr> LOGICAL_NOT
%token <ASTNode::ptr> LOGICAL_AND
%token <ASTNode::ptr>  LOGICAL_OR

 /* Control Flow */
%token <ASTNode::ptr> RETURN
%token <ASTNode::ptr> IF
%token <ASTNode::ptr> ELSE
%token <ASTNode::ptr> WHILE
%token <ASTNode::ptr> FOR
%token <ASTNode::ptr> CONTINUE
%token <ASTNode::ptr> GOTO
%token <ASTNode::ptr> LABEL
%token <ASTNode::ptr> BREAK
%token <ASTNode::ptr> SWITCH
%token <ASTNode::ptr> CASE
%token <ASTNode::ptr> DEFAULT

 /* Binary operations */
%token <ASTNode::ptr> DOUBLE_COLON
%token <ASTNode::ptr> PLUS
%token <ASTNode::ptr> MINUS
%token <ASTNode::ptr> SLASH
%token <ASTNode::ptr> PERCENT
%token <ASTNode::ptr> STAR
%token <ASTNode::ptr> EQUALS
%token <ASTNode::ptr> BANG
%token <ASTNode::ptr> TILDE
%token <ASTNode::ptr> ANDPERSAND
%token <ASTNode::ptr> MUL_ASSIGN
%token <ASTNode::ptr> DIV_ASSIGN
%token <ASTNode::ptr> MOD_ASSIGN
%token <ASTNode::ptr> ADD_ASSIGN
%token <ASTNode::ptr> SUB_ASSIGN
%token <ASTNode::ptr> LEFT_ASSIGN
%token <ASTNode::ptr> RIGHT_ASSIGN
%token <ASTNode::ptr> AND_ASSIGN
%token <ASTNode::ptr> XOR_ASSIGN
%token <ASTNode::ptr> OR_ASSIGN

%token <ASTNode::ptr>  LITERAL_CHAR
%token <ASTNode::ptr>  LITERAL_FLOAT
%token <ASTNode::ptr>  LITERAL_INT
%token <ASTNode::ptr>  LITERAL_STRING

%token NST_COMMENT_MULTILINE
%token NST_COMMENT_SINGLE_LINE
%token NST_WHITESPACE
%token NST_FILE_METADATA

%token <ASTNode::ptr> LT_OP
%token <ASTNode::ptr> GT_OP
%token <ASTNode::ptr> LE_OP
%token <ASTNode::ptr> GE_OP
%token <ASTNode::ptr> NE_OP
%token <ASTNode::ptr> EQ_OP

%token <ASTNode::ptr> XOR_OP
%token <ASTNode::ptr> AND_OP
%token <ASTNode::ptr> PIPE
%token <ASTNode::ptr> OR_OP
%token <ASTNode::ptr> LEFT_OP
%token <ASTNode::ptr> RIGHT_OP

/* Common punctuation */
%token <ASTNode::ptr> COMMA
%token <ASTNode::ptr> INC_OP
%token <ASTNode::ptr> DEC_OP
%token <ASTNode::ptr> PTR_OP
%token <ASTNode::ptr> DOT
%token <ASTNode::ptr> PAREN_L
%token <ASTNode::ptr> PAREN_R
%token <ASTNode::ptr> BRACE_L
%token <ASTNode::ptr> BRACKET_R
%token <ASTNode::ptr> BRACKET_L
%token <ASTNode::ptr> BRACE_R
%token <ASTNode::ptr> SEMICOLON
%token <ASTNode::ptr> COLON
%token <ASTNode::ptr> QUESTIONMARK

%token <ASTNode::ptr> PREC_FIRST
%token <ASTNode::ptr> PREC_SECOND

%nterm <ASTNode::ptr> syntax_file;

%nterm <ASTNode::ptr> opt_file_statement_list;
%nterm <ASTNode::ptr> file_statement_list;
%nterm <ASTNode::ptr> file_statement;
%nterm <ASTNode::ptr> file_statement_function_definition;
%nterm <ASTNode::ptr> file_statement_function_declaration;
%nterm <ASTNode::ptr> type_definition;
%nterm <ASTNode::ptr> opt_unsafe
%nterm <ASTNode::ptr> file_statement_namespace;
%nterm <ASTNode::ptr> file_statement_using;
%nterm <ASTNode::ptr> file_global_declaration;
%nterm <ASTNode::ptr> opt_global_initializer;
%nterm <ASTNode::ptr> global_initializer;
%nterm <ASTNode::ptr> opt_struct_initializer_list;
%nterm <ASTNode::ptr> struct_initializer_list;
%nterm <ASTNode::ptr> struct_initializer;
%nterm <ASTNode::ptr> access_modifier;

%nterm <ASTNode::ptr> scope_body;
%nterm <ASTNode::ptr> statement_list;
%nterm <ASTNode::ptr> statement;
%nterm <ASTNode::ptr> statement_variable_declaration;
%nterm <ASTNode::ptr> statement_expression;
%nterm <ASTNode::ptr> statement_block;
%nterm <ASTNode::ptr> statement_return;
%nterm <ASTNode::ptr> statement_break;
%nterm <ASTNode::ptr> statement_continue;
%nterm <ASTNode::ptr> statement_goto;
%nterm <ASTNode::ptr> statement_label;
%nterm <ASTNode::ptr> statement_ifelse;
%nterm <ASTNode::ptr> statement_while;
%nterm <ASTNode::ptr> statement_for;
%nterm <ASTNode::ptr> statement_switch;
%nterm <ASTNode::ptr> statement_switch_block;
%nterm <ASTNode::ptr> statement_switch_content;
%nterm <ASTNode::ptr> opt_statement_switch_content;

%nterm <ASTNode::ptr> opt_function_definition_arg_list;
%nterm <ASTNode::ptr> function_definition_arg_list;
%nterm <ASTNode::ptr> function_definition_arg

%nterm <ASTNode::ptr> expression_primary;
%nterm <ASTNode::ptr> expression_primary_identifier;
%nterm <ASTNode::ptr> expression_primary_literal_int;
%nterm <ASTNode::ptr> expression_primary_literal_float;
%nterm <ASTNode::ptr> expression_primary_literal_char;
%nterm <ASTNode::ptr> expression_primary_literal_string;
%nterm <ASTNode::ptr> expression_primary_nested;

%nterm <ASTNode::ptr> expression_postfix;
%nterm <ASTNode::ptr> expression_postfix_primary;
%nterm <ASTNode::ptr> expression_postfix_arrayindex;
%nterm <ASTNode::ptr> expression_postfix_function_call;
%nterm <ASTNode::ptr> expression_postfix_dot;
%nterm <ASTNode::ptr> expression_postfix_arrow;
%nterm <ASTNode::ptr> expression_postfix_increment;
%nterm <ASTNode::ptr> expression_postfix_decrement;

%nterm <ASTNode::ptr> expression_unary;
%nterm <ASTNode::ptr> expression_unary_increment;
%nterm <ASTNode::ptr> expression_unary_decrement;
%nterm <ASTNode::ptr> expression_unary_cast;
%nterm <ASTNode::ptr> expression_unary_sizeof_type;

%nterm <ASTNode::ptr> expression_cast;
%nterm <ASTNode::ptr> expression_cast_unary;
%nterm <ASTNode::ptr> expression_cast_cast;

%nterm <ASTNode::ptr> expression_multiplicative;
%nterm <ASTNode::ptr> expression_multiplicative_cast;
%nterm <ASTNode::ptr> expression_multiplicative_multiply;
%nterm <ASTNode::ptr> expression_multiplicative_divide;
%nterm <ASTNode::ptr> expression_multiplicative_modulo;

%nterm <ASTNode::ptr> expression_additive;
%nterm <ASTNode::ptr> expression_additive_multiplicative;
%nterm <ASTNode::ptr> expression_additive_plus;
%nterm <ASTNode::ptr> expression_additive_minus;

%nterm <ASTNode::ptr> expression_shift;
%nterm <ASTNode::ptr> expression_shift_additive;
%nterm <ASTNode::ptr> expression_shift_left;
%nterm <ASTNode::ptr> expression_shift_right;

%nterm <ASTNode::ptr> expression_relational;
%nterm <ASTNode::ptr> expression_relational_shift;
%nterm <ASTNode::ptr> expression_relational_gt;
%nterm <ASTNode::ptr> expression_relational_lt;
%nterm <ASTNode::ptr> expression_relational_le;
%nterm <ASTNode::ptr> expression_relational_ge;

%nterm <ASTNode::ptr> expression_equality;
%nterm <ASTNode::ptr> expression_and;
%nterm <ASTNode::ptr> expression_exclusive_or;
%nterm <ASTNode::ptr> expression_inclusive_or;
%nterm <ASTNode::ptr> expression_logical_and;
%nterm <ASTNode::ptr> expression_logical_or;

%nterm <ASTNode::ptr> expression_conditional;
%nterm <ASTNode::ptr> expression_assignment;

%nterm <ASTNode::ptr> type_specifier;
%nterm <ASTNode::ptr> type_name;
%nterm <ASTNode::ptr> type_name_qualified;
%nterm <ASTNode::ptr> type_access_qualifier;
%nterm <ASTNode::ptr> opt_array_length;

%nterm <ASTNode::ptr> struct_declaration_list;
%nterm <ASTNode::ptr> struct_declaration;

%nterm <ASTNode::ptr> operator_unary;
%nterm <ASTNode::ptr> operator_assignment;

%nterm <ASTNode::ptr> opt_argument_expression_list;
%nterm <ASTNode::ptr> argument_expression_list;

%nterm <ASTNode::ptr> expression;

%parse-param {return_data_t *return_data}

%code
{
  extern int lineno;
} // %code

%%

/*** Rules Section ***/
syntax_file
        : opt_file_statement_list YYEOF {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_syntax_file;
                $$->type_name = std::string("syntax_file");
                $$->children.push_back($1);
                $$->children.push_back($2);
                return_data->parsed = $$;
                PRINT_NONTERMINALS($$);
        }
        ;

opt_file_statement_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_list;
                $$->type_name = std::string("file_statement_list");
                PRINT_NONTERMINALS($$);
        }
        | file_statement_list {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_list 
        : file_statement {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_list;
                $$->type_name = std::string("file_statement_list");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | file_statement_list file_statement {
                $$ = $1;
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement
        : file_statement_function_definition {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | file_statement_function_declaration {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | file_global_declaration {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | type_definition {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | file_statement_namespace {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | file_statement_using {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

file_global_declaration
        : access_modifier type_specifier IDENTIFIER opt_array_length opt_global_initializer SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_global_declaration;
                $$->type_name = std::string("file_global_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_global_initializer
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_global_declaration;
                $$->type_name = std::string("global_initializer");
        }
        | global_initializer {
                $$ = $1;
        }
        ;

global_initializer
        : EQUALS expression_primary {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->type_name = std::string("global_initializer");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        | EQUALS ANDPERSAND expression_primary {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->type_name = std::string("global_initializer");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        | EQUALS BRACE_L opt_struct_initializer_list BRACE_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->type_name = std::string("global_initializer");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
        }
        ;


opt_struct_initializer_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_initializer_list;
                $$->type_name = std::string("struct_initializer_list");
        }
        | struct_initializer_list {
                $$ = $1;
        }
        ;

struct_initializer_list
        : struct_initializer {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_initializer_list;
                $$->type_name = std::string("struct_initializer_list");
                $$->children.push_back($1);
        }
        | struct_initializer_list struct_initializer {
                $$ = $1;
                $$->children.push_back($2);
        }
        ;

struct_initializer
        : DOT IDENTIFIER global_initializer SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->type_name = std::string("struct_initializer");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
        }
        ;

access_modifier
        : PUBLIC {
                $$ = $1;
        }
        | PRIVATE {
                $$ = $1;
        }
        | PROTECTED {
                $$ = $1;
        }
        ;

file_statement_namespace
        : NAMESPACE IDENTIFIER BRACE_L opt_file_statement_list BRACE_R SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_namespace;
                $$->type_name = std::string("file_statement_namespace");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
        }
        ;

file_statement_using
        : USING NAMESPACE type_name_qualified SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_using;
                $$->type_name = std::string("file_statement_using");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
        }
        ;

type_definition
        : TYPEDEF type_specifier IDENTIFIER SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_definition;
                $$->type_name = std::string("type_definition");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_unsafe
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_opt_unsafe;
                $$->type_name = std::string("opt_unsafe");
        }
        | UNSAFE {
                $$ = $1;
        }
        ;

file_statement_function_declaration
: opt_unsafe type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_function_definition;
                $$->type_name = std::string("file_statement_function_definition");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                PRINT_NONTERMINALS($$);
}
;

file_statement_function_definition
        : opt_unsafe type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_function_definition;
                $$->type_name = std::string("file_statement_function_definition");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_function_definition_arg_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_function_definition_arg_list;
                $$->type_name = std::string("function_definition_arg_list");
        }
        | function_definition_arg_list {
                $$ = $1;
        }
        ;

function_definition_arg_list
        : function_definition_arg {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_function_definition_arg_list;
                $$->type_name = std::string("function_definition_arg_list");
                $$->children.push_back($1);
        }
        | function_definition_arg_list COMMA function_definition_arg {
                $$ = $1;
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;
function_definition_arg
        : type_specifier IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_function_definition_arg;
                $$->type_name = std::string("function_definition_arg");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        ;

scope_body
        : BRACE_L statement_list BRACE_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_scope_body;
                $$->type_name = std::string("scope_body");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_scope_body;
                $$->type_name = std::string("statement_list");
        }
        | statement_list statement {
                $$ = $1;
                $$->children.push_back($2);
        }
        ;

statement
        : statement_block {
                $$ = $1;
        }
        | statement_ifelse {
                $$ = $1;
        }
        | statement_while {
                $$ = $1;
        }
        | statement_for {
                $$ = $1;
        }
        | statement_switch {
                $$ = $1;
        }
        | statement_return {
                $$ = $1;
        }
        | type_definition {
                $$ = $1;
        }
        | statement_continue {
                $$ = $1;
        }
        | statement_goto {
                $$ = $1;
        }
        | statement_break {
                $$ = $1;
        }
        | statement_label {
                $$ = $1;
        }
        | statement_expression {
                $$ = $1;
        }
        | statement_variable_declaration {
                $$ = $1;
        }
        ;

opt_array_length
: /**/ {
        $$ = std::make_shared<ASTNode>();
        $$->type = Parser::symbol_kind_type::S_opt_array_length;
        $$->type_name = std::string("opt_array_length");
}
| BRACKET_L LITERAL_INT BRACKET_R {
        $$ = std::make_shared<ASTNode>();
        $$->type = Parser::symbol_kind_type::S_opt_array_length;
        $$->type_name = std::string("opt_array_length");
        $$->children.push_back($1);
        $$->children.push_back($2);
        $$->children.push_back($3);
}
;

statement_variable_declaration
        : VAR type_specifier IDENTIFIER opt_array_length opt_global_initializer SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_variable_declaration;
                $$->type_name = std::string("statement_variable_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
        }
        ;

statement_block
        : opt_unsafe scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_block;
                $$->type_name = std::string("statement_block");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        ;

statement_expression
        : expression SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_expression;
                $$->type_name = std::string("statement_expression");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        ;
statement_goto
        : GOTO IDENTIFIER SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_goto;
                $$->type_name = std::string("statement_goto");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;
statement_break
        : BREAK SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_break;
                 $$->type_name = std::string("statement_break");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        ;
statement_continue
        : CONTINUE SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_continue;
                $$->type_name = std::string("statement_continue");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        ;
statement_label
        : LABEL IDENTIFIER COLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_label;
                $$->type_name = std::string("statement_label");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;

statement_return
        : RETURN expression SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_return;
                $$->type_name = std::string("statement_return");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ; 

statement_ifelse
        : IF PAREN_L expression PAREN_R scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_ifelse;
                $$->type_name = std::string("statement_ifelse");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
        }
        | IF PAREN_L expression PAREN_R scope_body ELSE statement_ifelse {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_ifelse;
                $$->type_name = std::string("statement_ifelse");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
        }
        | IF PAREN_L expression PAREN_R scope_body ELSE scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_ifelse;
                $$->type_name = std::string("statement_ifelse");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
        }
        ;

statement_while
        : WHILE PAREN_L expression PAREN_R scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_while;
                $$->type_name = std::string("statement_while");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
        }
        ;

statement_for
        : FOR PAREN_L expression SEMICOLON expression SEMICOLON expression PAREN_R scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_for;
                $$->type_name = std::string("statement_for");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                $$->children.push_back($8);
                $$->children.push_back($9);
        }
        ;

statement_switch
        : SWITCH PAREN_L expression PAREN_R BRACE_L opt_statement_switch_content BRACE_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch;
                $$->type_name = std::string("statement_switch");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
        }

opt_statement_switch_content
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch_content;
                $$->type_name = std::string("statement_switch_content");
        }
        | statement_switch_content {
                $$ = $1;
        }
        ;

statement_switch_content
        : statement_switch_block {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch_content;
                $$->type_name = std::string("statement_switch_content");
                $$->children.push_back($1);
        }
        | statement_switch_content statement_switch_block {
                $$ = $1;
                $$->children.push_back($2);
        }
        ;

statement_switch_block
        : DEFAULT COLON scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch_content;
                $$->type_name = std::string("statement_switch_block");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        | CASE expression COLON scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch_content;
                $$->type_name = std::string("statement_switch_block");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
        }
        ;

expression_primary
        : expression_primary_nested {
                $$ = $1;
        }
        | expression_primary_identifier {
                $$ = $1;
        }
        | expression_primary_literal_int {
                $$ = $1;
        }
        | expression_primary_literal_float {
                $$ = $1;
        }
        | expression_primary_literal_char {
                $$ = $1;
        }
        | expression_primary_literal_string {
                $$ = $1;
        }
        ;

expression_primary_identifier
        : IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_identifier;
                $$->type_name = std::string("expression_primary_identifier");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_literal_int
        : LITERAL_INT {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_literal_int;
                $$->type_name = std::string("expression_primary_literal_int");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_float
        : LITERAL_FLOAT {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_literal_float;
                $$->type_name = std::string("expression_primary_literal_float");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_char
        : LITERAL_CHAR {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_literal_char;
                $$->type_name = std::string("expression_primary_literal_char");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_string
        : LITERAL_STRING {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_literal_string;
                $$->type_name = std::string("expression_primary_literal_string");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_nested
        : PAREN_L expression PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_nested;
                $$->type_name = std::string("expression_primary_literal_nested");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

expression_postfix
        : expression_postfix_primary {
                $$ = $1;
        }
        | expression_postfix_arrayindex {
                $$ = $1;
        }
        | expression_postfix_function_call {
                $$ = $1;
        }
        | expression_postfix_dot {
                $$ = $1;
        }
        | expression_postfix_arrow {
                $$ = $1;
        }
        | expression_postfix_increment {
                $$ = $1;
        }
        | expression_postfix_decrement {
                $$ = $1;
        }
        ;

expression_postfix_primary
        : expression_primary {
                $$ = $1;
        }
        ;

expression_postfix_arrayindex
        : expression_postfix BRACKET_L expression BRACKET_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_postfix_arrayindex;
                $$->type_name = std::string("expression_postfix_arrayindex");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_function_call
        : expression_postfix PAREN_L opt_argument_expression_list PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_postfix_function_call;
                $$->type_name = std::string("expression_postfix_function_call");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_dot
        : expression_postfix DOT IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_postfix_dot;
                $$->type_name = std::string("expression_postfix_dot");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_arrow
        : expression_postfix PTR_OP IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_postfix_arrow;
                $$->type_name = std::string("expression_postfix_arrow");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_increment
        : expression_postfix INC_OP {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_postfix_increment;
                $$->type_name = std::string("expression_postfix_increment");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_decrement
        : expression_postfix DEC_OP {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_postfix_decrement;
                $$->type_name = std::string("expression_postfix_decrement");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
	;

expression_unary
        : expression_postfix {
                $$ = $1;
        }
        | expression_unary_increment {
                $$ = $1;
        }
        | expression_unary_decrement {
                $$ = $1;
        }
        | expression_unary_cast {
                $$ = $1;
        }
        | expression_unary_sizeof_type {
                $$ = $1;
        }
        ;
expression_unary_increment
        : INC_OP expression_unary {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_unary_increment;
                $$->type_name = std::string("expression_unary_increment");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        ;
expression_unary_decrement
        : DEC_OP expression_unary {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_unary_decrement;
                $$->type_name = std::string("expression_unary_decrement");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        ;
expression_unary_cast
        : operator_unary expression_cast {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_unary_cast;
                $$->type_name = std::string("expression_unary_cast");
                $$->children.push_back($1);
                $$->children.push_back($2);
        }
        ;
expression_unary_sizeof_type
        : SIZEOF PAREN_L type_specifier PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_unary_sizeof_type;
                $$->type_name = std::string("expression_unary_sizeof_type");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
        }
        ;



operator_unary
        : ANDPERSAND {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->type_name = std::string("operator_unary");
                $$->children.push_back($1);
        }
        | STAR {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->type_name = std::string("operator_unary");
                $$->children.push_back($1);
        }
        | PLUS {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->type_name = std::string("operator_unary");
                $$->children.push_back($1);
        }
        | MINUS {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->type_name = std::string("operator_unary");
                $$->children.push_back($1);
        }
        | TILDE {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->type_name = std::string("operator_unary");
                $$->children.push_back($1);
        }
        | BANG {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->type_name = std::string("operator_unary");
                $$->children.push_back($1);
        }
	;

expression_cast
        : expression_cast_unary {
                $$ = $1;
        }
        | expression_cast_cast {
                $$ = $1;
        }
        ;

expression_cast_unary
        : expression_unary {
                $$ = $1;
        }
        ;

expression_cast_cast
        : CAST PAREN_L type_specifier COMMA expression PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_cast_cast;
                $$->type_name = std::string("expression_cast_cast");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
        }
	;


expression_multiplicative
        : expression_multiplicative_cast {
                $$ = $1;
        }
        | expression_multiplicative_multiply {
                $$ = $1;
        }
        | expression_multiplicative_divide {
                $$ = $1;
        }
        | expression_multiplicative_modulo {
                $$ = $1;
        }
        ;

expression_multiplicative_cast
        : expression_cast {
                $$ = $1;
        }
        ;
expression_multiplicative_multiply
        : expression_multiplicative STAR expression_cast {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_multiplicative_multiply;
                $$->type_name = std::string("expression_multiplicative_multiply");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;
expression_multiplicative_divide
        : expression_multiplicative SLASH expression_cast {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_multiplicative_divide;
                $$->type_name = std::string("expression_multiplicative_divide");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($1);
        }
        ;
expression_multiplicative_modulo
        : expression_multiplicative PERCENT expression_cast {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_multiplicative_modulo;
                $$->type_name = std::string("expression_multiplicative_modulo");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;

expression_additive
        : expression_additive_multiplicative {
                $$ = $1;
        }
        | expression_additive_plus {
                $$ = $1;
        }
        | expression_additive_minus {
                $$ = $1;
        }
        ;
expression_additive_multiplicative
        : expression_multiplicative {
                $$ = $1;
        }
        ;
expression_additive_plus
        : expression_additive PLUS expression_multiplicative {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_additive_plus;
                $$->type_name = std::string("expression_additive_plus");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;
expression_additive_minus
        : expression_additive MINUS expression_multiplicative {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_additive_minus;
                $$->type_name = std::string("expression_additive_minus");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;

expression_shift
        : expression_shift_additive {
                $$ = $1;
        }
        | expression_shift_left {
                $$ = $1;
        }
        | expression_shift_right {
                $$ = $1;
        }
        ;

expression_shift_additive
        : expression_additive {
                $$ = $1;
        }
        ;
expression_shift_left
        : expression_shift LEFT_OP expression_additive {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_shift_left;
                $$->type_name = std::string("expression_shift_left");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;
expression_shift_right
        : expression_shift RIGHT_OP expression_additive {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_shift_right;
                $$->type_name = std::string("expression_shift_right");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;


expression_relational
        : expression_relational_shift {
                $$ = $1;
        }
        | expression_relational_lt {
                $$ = $1;
        }
        | expression_relational_gt {
                $$ = $1;
        }
        | expression_relational_le {
                $$ = $1;
        }
        | expression_relational_ge {
                $$ = $1;
        }
        ;

expression_relational_shift
        : expression_shift {
                $$ = $1;
        }
        ;
expression_relational_lt
        : expression_relational LT_OP expression_shift {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_relational_lt;
                $$->type_name = std::string("expression_relational_lt");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;
expression_relational_gt
        : expression_relational GT_OP expression_shift {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_relational_gt;
                $$->type_name = std::string("expression_relational_gt");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;
expression_relational_le
        : expression_relational LE_OP expression_shift {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_relational_le;
                $$->type_name = std::string("expression_relational_le");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;
expression_relational_ge
        : expression_relational GE_OP expression_shift {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_relational_ge;
                $$->type_name = std::string("expression_relational_ge");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;

expression_equality
        : expression_relational {
                $$ = $1;
        }
        | expression_equality EQ_OP expression_relational {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_equality;
                $$->type_name = std::string("expression_equality");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        | expression_equality NE_OP expression_relational {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_equality;
                $$->type_name = std::string("expression_equality");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
	;

expression_and
        : expression_equality {
                $$ = $1;
        }
        | expression_and ANDPERSAND expression_equality {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_and;
                $$->type_name = std::string("expression_and");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
	;

expression_exclusive_or
        : expression_and {
                $$ = $1;
        }
        | expression_exclusive_or XOR_OP expression_and {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_exclusive_or;
                $$->type_name = std::string("expression_exclusive_or");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
	;

expression_inclusive_or
        : expression_exclusive_or {
                $$ = $1;
        }
        | expression_inclusive_or PIPE expression_exclusive_or {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_inclusive_or;
                $$->type_name = std::string("expression_exclusive_or");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
	;

expression_logical_and
        : expression_inclusive_or {
                $$ = $1;
        }
        | expression_logical_and AND_OP expression_inclusive_or {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_logical_and;
                $$->type_name = std::string("expression_logical_and");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
	;

expression_logical_or
        : expression_logical_and {
                $$ = $1;
        }
        | expression_logical_or OR_OP expression_logical_and {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_logical_or;
                $$->type_name = std::string("expression_logical_or");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
	;

expression_conditional
        : expression_logical_or {
                $$ = $1;
        }
        | expression_logical_or QUESTIONMARK expression COLON expression_conditional {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_conditional;
                $$->type_name = std::string("expression_conditional");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
        }
	;

expression_assignment
        : expression_conditional {
                $$ = $1;
        }
        | expression_unary operator_assignment expression_assignment {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_assignment;
                $$->type_name = std::string("expression_assignment");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
	;

operator_assignment
        : EQUALS {
                $$ = $1;
        }
	| MUL_ASSIGN {
                $$ = $1;
        }
	| DIV_ASSIGN {
                $$ = $1;
        }
	| MOD_ASSIGN {
                $$ = $1;
        }
	| ADD_ASSIGN {
                $$ = $1;
        }
	| SUB_ASSIGN {
                $$ = $1;
        }
	| LEFT_ASSIGN {
                $$ = $1;
        }
	| RIGHT_ASSIGN {
                $$ = $1;
        }
	| AND_ASSIGN {
                $$ = $1;
        } 
	| XOR_ASSIGN {
                $$ = $1;
        }
	| OR_ASSIGN {
                $$ = $1;
        }
	;

expression
        : expression_assignment {
                $$ = $1;
        }
        ;


type_name
        : STRUCT BRACE_L struct_declaration_list BRACE_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_name;
                $$->type_name = std::string("type_name");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
        }
        | TYPEOF PAREN_L expression PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_name;
                $$->type_name = std::string("type_name");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        | type_name_qualified {
                $$ = $1;
        }
        ;

type_name_qualified
        : IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_name_qualified;
                $$->type_name = std::string("type_name_qualified");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | type_name_qualified DOUBLE_COLON IDENTIFIER {
                $$ = $1;
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

struct_declaration_list
        : struct_declaration {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_declaration;
                $$->type_name = std::string("struct_declaration_list");
                $$->children.push_back($1);
        }
        | struct_declaration_list struct_declaration {
                $$ = $1;
                $$->children.push_back($2);
        }
        ;

struct_declaration
        : access_modifier type_specifier IDENTIFIER opt_array_length SEMICOLON {
                // Member
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_declaration;
                $$->type_name = std::string("struct_declaration_member");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
        }
        | access_modifier type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                // Method
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_declaration;
                $$->type_name = std::string("struct_declaration_method");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
        }
        | access_modifier PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
          // Constructor
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_declaration;
                $$->type_name = std::string("struct_declaration_constructor");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
        }
        | access_modifier TILDE PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
          // Destructor
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_declaration;
                $$->type_name = std::string("struct_declaration_destructor");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
        }
        ;


type_access_qualifier
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_access_qualifier;
                $$->type_name = std::string("type_access_qualifier");
        }
        | CONST {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_access_qualifier;
                $$->type_name = std::string("type_access_qualifier");
                $$->children.push_back($1);
        }
        | VOLATILE {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_access_qualifier;
                $$->type_name = std::string("type_access_qualifier");
                $$->children.push_back($1);
        }
        ;

type_specifier
        : type_access_qualifier type_name {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->type_name = std::string("type_specifier");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        | FUNCTION type_specifier PAREN_L opt_function_definition_arg_list PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->type_name = std::string("type_specifier");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier STAR type_access_qualifier {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->type_name = std::string("type_specifier-pointer-to");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier ANDPERSAND type_access_qualifier {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->type_name = std::string("type_specifier-reference-to");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_argument_expression_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_argument_expression_list;
                $$->type_name = std::string("argument_expression_list");
        }
        | argument_expression_list {
                $$ = $1;
        }
        ;

argument_expression_list
        : expression {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_argument_expression_list;
                $$->type_name = std::string("argument_expression_list");
                $$->children.push_back($1);
        }
        | argument_expression_list COMMA expression {
                $$ = $1;
                $$->children.push_back($2);
                $$->children.push_back($3);
        }
        ;

%%
 
void calc::Parser::error(const std::string& msg) {
    printf("Syntax error at line %d : %s\n", lineno, msg.c_str());
}
