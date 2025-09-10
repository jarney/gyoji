%{
#include <iostream>
#include <string>
#include <cmath>
#include <memory>
#include "target/jlang.l.hpp"
#include "jsyntax.hpp"
#include "ast.hpp"
#include "namespace.hpp"
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
#define PRINT_NONTERMINALS(s) printf("%s\n", s->typestr.c_str())
#else
#define PRINT_NONTERMINALS(s) (0)
#endif

  int visibility_from_modifier(ASTNode::ptr node);
  
#define YY_DECL                                                         \
        int yylex(calc::Parser::semantic_type *yylval, yyscan_t yyscanner)
    YY_DECL;
}

%token INVALID_INPUT
%token <ASTNode::ptr> YYEOF
%token <ASTNode::ptr> IDENTIFIER
%token <ASTNode::ptr> NAMESPACE_NAME
%token <ASTNode::ptr> TYPE_NAME
%token <ASTNode::ptr> CLASS
%token <ASTNode::ptr> ENUM

%token <ASTNode::ptr> NAMESPACE
%token <ASTNode::ptr> AS
%token <ASTNode::ptr> USING
%token <ASTNode::ptr> TYPEDEF
%token <ASTNode::ptr> STRUCT
%token <ASTNode::ptr> UNION
%token <ASTNode::ptr> SIZEOF
%token <ASTNode::ptr> TYPEOF
%token <ASTNode::ptr> CAST
%token <ASTNode::ptr> UNSAFE
 //%token <ASTNode::ptr> VAR
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
%nterm <ASTNode::ptr> class_definition;
%nterm <ASTNode::ptr> class_decl_start;
%nterm <ASTNode::ptr> opt_class_argument_list;
%nterm <ASTNode::ptr> class_argument_list;


%nterm <ASTNode::ptr> enum_definition;
%nterm <ASTNode::ptr> opt_enum_value_list;
%nterm <ASTNode::ptr> enum_value_list;
%nterm <ASTNode::ptr> enum_value;

%nterm <ASTNode::ptr> opt_unsafe
%nterm <ASTNode::ptr> namespace_declaration;
%nterm <ASTNode::ptr> file_statement_namespace;
%nterm <ASTNode::ptr> opt_as;
%nterm <ASTNode::ptr> file_statement_using;
%nterm <ASTNode::ptr> file_global_declaration;
%nterm <ASTNode::ptr> opt_global_initializer;
%nterm <ASTNode::ptr> global_initializer;
%nterm <ASTNode::ptr> opt_struct_initializer_list;
%nterm <ASTNode::ptr> struct_initializer_list;
%nterm <ASTNode::ptr> struct_initializer;
%nterm <ASTNode::ptr> opt_access_modifier;
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
%nterm <ASTNode::ptr> type_specifier_call_args;
%nterm <ASTNode::ptr> type_name;
%nterm <ASTNode::ptr> type_access_qualifier;
%nterm <ASTNode::ptr> opt_array_length;

%nterm <ASTNode::ptr> opt_class_member_declaration_list;
%nterm <ASTNode::ptr> class_member_declaration_list;
%nterm <ASTNode::ptr> class_member_declaration;

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
                $$->typestr = std::string("syntax_file");
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
                $$->typestr = std::string("file_statement_list");
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
                $$->typestr = std::string("file_statement_list");
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
        | class_definition {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | enum_definition {
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
        : opt_access_modifier opt_unsafe type_specifier IDENTIFIER opt_array_length opt_global_initializer SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_global_declaration;
                $$->typestr = std::string("file_global_declaration");
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

opt_global_initializer
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->typestr = std::string("global_initializer");
                PRINT_NONTERMINALS($$);
        }
        | global_initializer {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

global_initializer
        : EQUALS expression_primary {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->typestr = std::string("global_initializer");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        | EQUALS ANDPERSAND expression_primary {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->typestr = std::string("global_initializer");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        | EQUALS BRACE_L opt_struct_initializer_list BRACE_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->typestr = std::string("global_initializer");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        ;


opt_struct_initializer_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_initializer_list;
                $$->typestr = std::string("struct_initializer_list");
                PRINT_NONTERMINALS($$);
        }
        | struct_initializer_list {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

struct_initializer_list
        : struct_initializer {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_struct_initializer_list;
                $$->typestr = std::string("struct_initializer_list");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | struct_initializer_list struct_initializer {
                $$ = $1;
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

struct_initializer
        : DOT IDENTIFIER global_initializer SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_global_initializer;
                $$->typestr = std::string("struct_initializer");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_access_modifier
        :  {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_access_modifier;
                $$->typestr = std::string("access_modifier");
                PRINT_NONTERMINALS($$);
        }
        | access_modifier {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_access_modifier;
                $$->typestr = std::string("access_modifier");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;

access_modifier
        : PUBLIC {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | PRIVATE {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | PROTECTED {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

namespace_declaration
        : opt_access_modifier NAMESPACE IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_namespace_declaration;
                $$->typestr = std::string("namespace_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                return_data->namespace_context.namespace_new($3->value, Namespace::TYPE_NAMESPACE, visibility_from_modifier($1));
                return_data->namespace_context.namespace_push($3->value);
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_namespace
        : namespace_declaration BRACE_L opt_file_statement_list BRACE_R SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_namespace;
                $$->typestr = std::string("file_statement_namespace");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                return_data->namespace_context.namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

opt_as
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_opt_as;
                $$->typestr = std::string("opt_as");
                PRINT_NONTERMINALS($$);
        }
        | AS IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_opt_as;
                $$->typestr = std::string("opt_as");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_using
        : opt_access_modifier USING NAMESPACE NAMESPACE_NAME opt_as SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_using;
                $$->typestr = std::string("file_statement_using");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                NamespaceFoundReason::ptr found_std = return_data->namespace_context.namespace_lookup($4->value);
                if (!found_std) {
                  fprintf(stderr, "Error: no such namespace %s in using statement\n", $4->value.c_str());
                  exit(1);
                }
                if ($5->children.size() == 2) {
                  return_data->namespace_context.namespace_using($5->children.back()->value, found_std->location);
                }
                else {
                  return_data->namespace_context.namespace_using("", found_std->location);
                }
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier USING NAMESPACE TYPE_NAME opt_as SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_using;
                $$->typestr = std::string("file_statement_using");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                NamespaceFoundReason::ptr found_std = return_data->namespace_context.namespace_lookup($4->value);
                if (!found_std) {
                  fprintf(stderr, "Error: no such namespace %s in using statement\n", $4->value.c_str());
                  exit(1);
                }
                if ($5->children.size() == 2) {
                  return_data->namespace_context.namespace_using($5->children.back()->value, found_std->location);
                }
                else {
                  return_data->namespace_context.namespace_using("", found_std->location);
                }
                PRINT_NONTERMINALS($$);
        }
        ;

class_decl_start
        : opt_access_modifier CLASS IDENTIFIER opt_class_argument_list {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_decl_start;
                $$->typestr = std::string("class_decl_start");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                return_data->namespace_context.namespace_new($3->value, Namespace::TYPE_CLASS, visibility_from_modifier($1));
                return_data->namespace_context.namespace_push($3->value);
                if ($4->children.size() > 0) {
                  for (auto child : $4->children) {
                    if (child->type == Parser::symbol_kind_type::S_class_argument_list) {
                      for (auto grandchild : child->children) {
                        if (grandchild->type == Parser::symbol_kind_type::S_IDENTIFIER) {
                          return_data->namespace_context.namespace_new(grandchild->value, Namespace::TYPE_CLASS, Namespace::VISIBILITY_PRIVATE);
                        }
                      }
                    }
                  }
                }
                PRINT_NONTERMINALS($$);
        }
        ;

opt_class_argument_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_opt_class_argument_list;
                $$->typestr = std::string("opt_class_argument_list");
                PRINT_NONTERMINALS($$);
        }
        | PAREN_L class_argument_list PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_opt_class_argument_list;
                $$->typestr = std::string("opt_class_argument_list");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

// At this stage, the arguments
// are identifiers.  Once they are
// recognized, they are turned into
// types scoped private in the class.
class_argument_list
        : IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_argument_list;
                $$->typestr = std::string("class_argument_list");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | class_argument_list COMMA IDENTIFIER {
                $$ = $1;
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

class_definition
        : class_decl_start BRACE_L opt_class_member_declaration_list BRACE_R SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_definition;
                $$->typestr = std::string("class_definition");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                return_data->namespace_context.namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

type_definition
        : opt_access_modifier TYPEDEF type_specifier IDENTIFIER SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_definition;
                $$->typestr = std::string("type_definition");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                return_data->namespace_context.namespace_new($4->value, Namespace::TYPE_TYPEDEF, visibility_from_modifier($1));
                PRINT_NONTERMINALS($$);
        }
        ;

enum_definition
        : opt_access_modifier ENUM type_name IDENTIFIER BRACE_L opt_enum_value_list BRACE_R SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_enum_definition;
                $$->typestr = std::string("enum_definition");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                $$->children.push_back($8);
                return_data->namespace_context.namespace_new($4->value, Namespace::TYPE_TYPEDEF, visibility_from_modifier($1));
                PRINT_NONTERMINALS($$);
        }
        ;

opt_enum_value_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_enum_value_list;
                $$->typestr = std::string("enum_value_list");
                PRINT_NONTERMINALS($$);
        }
        | enum_value_list {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

enum_value_list
        : enum_value {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_enum_value_list;
                $$->typestr = std::string("enum_value_list");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | enum_value_list enum_value {
                $$ = $1;
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

enum_value
        : IDENTIFIER EQUALS expression_primary SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_enum_value;
                $$->typestr = std::string("enum_value");
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
                $$->typestr = std::string("opt_unsafe");
                PRINT_NONTERMINALS($$);
        }
        | UNSAFE {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_function_declaration
        : opt_access_modifier opt_unsafe type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_function_declaration;
                $$->typestr = std::string("file_statement_function_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                $$->children.push_back($8);
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_function_definition
        : opt_access_modifier opt_unsafe type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_file_statement_function_definition;
                $$->typestr = std::string("file_statement_function_definition");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                $$->children.push_back($8);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_function_definition_arg_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_function_definition_arg_list;
                $$->typestr = std::string("function_definition_arg_list");
                PRINT_NONTERMINALS($$);
        }
        | function_definition_arg_list {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

function_definition_arg_list
        : function_definition_arg {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_function_definition_arg_list;
                $$->typestr = std::string("function_definition_arg_list");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | function_definition_arg_list COMMA function_definition_arg {
                $$ = $1;
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;
function_definition_arg
        : type_specifier IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_function_definition_arg;
                $$->typestr = std::string("function_definition_arg");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

scope_body
        : BRACE_L statement_list BRACE_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_scope_body;
                $$->typestr = std::string("scope_body");
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
                $$->typestr = std::string("statement_list");
                PRINT_NONTERMINALS($$);
        }
        | statement_list statement {
                $$ = $1;
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

statement
        : statement_block {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_ifelse {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_while {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_for {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_switch {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_return {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_continue {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_goto {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_break {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_label {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_expression {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | statement_variable_declaration {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

opt_array_length
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_opt_array_length;
                $$->typestr = std::string("opt_array_length");
                PRINT_NONTERMINALS($$);
        }
        | BRACKET_L LITERAL_INT BRACKET_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_opt_array_length;
                $$->typestr = std::string("opt_array_length");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_variable_declaration
        : type_specifier IDENTIFIER opt_array_length opt_global_initializer SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_variable_declaration;
                $$->typestr = std::string("statement_variable_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_block
        : opt_unsafe scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_block;
                $$->typestr = std::string("statement_block");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_expression
        : expression SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_expression;
                $$->typestr = std::string("statement_expression");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;
statement_goto
        : GOTO IDENTIFIER SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_goto;
                $$->typestr = std::string("statement_goto");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;
statement_break
        : BREAK SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_break;
                 $$->typestr = std::string("statement_break");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;
statement_continue
        : CONTINUE SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_continue;
                $$->typestr = std::string("statement_continue");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;
statement_label
        : LABEL IDENTIFIER COLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_label;
                $$->typestr = std::string("statement_label");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_return
        : RETURN expression SEMICOLON {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_return;
                $$->typestr = std::string("statement_return");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ; 

statement_ifelse
        : IF PAREN_L expression PAREN_R scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_ifelse;
                $$->typestr = std::string("statement_ifelse");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                PRINT_NONTERMINALS($$);
        }
        | IF PAREN_L expression PAREN_R scope_body ELSE statement_ifelse {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_ifelse;
                $$->typestr = std::string("statement_ifelse");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                PRINT_NONTERMINALS($$);
        }
        | IF PAREN_L expression PAREN_R scope_body ELSE scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_ifelse;
                $$->typestr = std::string("statement_ifelse");
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

statement_while
        : WHILE PAREN_L expression PAREN_R scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_while;
                $$->typestr = std::string("statement_while");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_for
        : FOR PAREN_L expression SEMICOLON expression SEMICOLON expression PAREN_R scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_for;
                $$->typestr = std::string("statement_for");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                $$->children.push_back($8);
                $$->children.push_back($9);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_switch
        : SWITCH PAREN_L expression PAREN_R BRACE_L opt_statement_switch_content BRACE_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch;
                $$->typestr = std::string("statement_switch");
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

opt_statement_switch_content
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch_content;
                $$->typestr = std::string("statement_switch_content");
                PRINT_NONTERMINALS($$);
        }
        | statement_switch_content {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

statement_switch_content
        : statement_switch_block {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch_content;
                $$->typestr = std::string("statement_switch_content");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | statement_switch_content statement_switch_block {
                $$ = $1;
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_switch_block
        : DEFAULT COLON scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch_block;
                $$->typestr = std::string("statement_switch_block");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        | CASE expression COLON scope_body {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_statement_switch_block;
                $$->typestr = std::string("statement_switch_block");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary
        : expression_primary_nested {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_identifier {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_int {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_float {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_char {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_string {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_identifier
        : IDENTIFIER {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_identifier;
                $$->typestr = std::string("expression_primary_identifier");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_literal_int
        : LITERAL_INT {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_literal_int;
                $$->typestr = std::string("expression_primary_literal_int");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_float
        : LITERAL_FLOAT {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_literal_float;
                $$->typestr = std::string("expression_primary_literal_float");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_char
        : LITERAL_CHAR {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_literal_char;
                $$->typestr = std::string("expression_primary_literal_char");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_string
        : LITERAL_STRING {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_literal_string;
                $$->typestr = std::string("expression_primary_literal_string");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_nested
        : PAREN_L expression PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_primary_nested;
                $$->typestr = std::string("expression_primary_literal_nested");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

expression_postfix
        : expression_postfix_primary {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_arrayindex {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_function_call {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_dot {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_arrow {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_increment {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_decrement {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_primary
        : expression_primary {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_arrayindex
        : expression_postfix BRACKET_L expression BRACKET_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_postfix_arrayindex;
                $$->typestr = std::string("expression_postfix_arrayindex");
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
                $$->typestr = std::string("expression_postfix_function_call");
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
                $$->typestr = std::string("expression_postfix_dot");
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
                $$->typestr = std::string("expression_postfix_arrow");
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
                $$->typestr = std::string("expression_postfix_increment");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_decrement
        : expression_postfix DEC_OP {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_expression_postfix_decrement;
                $$->typestr = std::string("expression_postfix_decrement");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
	;

expression_unary
        : expression_postfix {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_unary_increment {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_unary_decrement {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_unary_cast {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_unary_sizeof_type {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_unary_increment
        : INC_OP expression_unary {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_unary_increment;
                $$->typestr = std::string("expression_unary_increment");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_decrement
        : DEC_OP expression_unary {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_unary_decrement;
                $$->typestr = std::string("expression_unary_decrement");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_cast
        : operator_unary expression_cast {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_unary_cast;
                $$->typestr = std::string("expression_unary_cast");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_sizeof_type
        : SIZEOF PAREN_L type_specifier PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_unary_sizeof_type;
                $$->typestr = std::string("expression_unary_sizeof_type");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        ;



operator_unary
        : ANDPERSAND {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->typestr = std::string("operator_unary");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | STAR {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->typestr = std::string("operator_unary");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | PLUS {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->typestr = std::string("operator_unary");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | MINUS {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->typestr = std::string("operator_unary");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | TILDE {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->typestr = std::string("operator_unary");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | BANG {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_operator_unary;
                $$->typestr = std::string("operator_unary");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
	;

expression_cast
        : expression_cast_unary {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_cast_cast {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_cast_unary
        : expression_unary {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_cast_cast
        : CAST PAREN_L type_specifier COMMA expression PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_cast_cast;
                $$->typestr = std::string("expression_cast_cast");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                PRINT_NONTERMINALS($$);
        }
	;


expression_multiplicative
        : expression_multiplicative_cast {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_multiplicative_multiply {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_multiplicative_divide {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_multiplicative_modulo {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_multiplicative_cast
        : expression_cast {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;
expression_multiplicative_multiply
        : expression_multiplicative STAR expression_cast {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_multiplicative_multiply;
                $$->typestr = std::string("expression_multiplicative_multiply");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_multiplicative_divide
        : expression_multiplicative SLASH expression_cast {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_multiplicative_divide;
                $$->typestr = std::string("expression_multiplicative_divide");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_multiplicative_modulo
        : expression_multiplicative PERCENT expression_cast {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_multiplicative_modulo;
                $$->typestr = std::string("expression_multiplicative_modulo");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_additive
        : expression_additive_multiplicative {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_additive_plus {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_additive_minus {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;
expression_additive_multiplicative
        : expression_multiplicative {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;
expression_additive_plus
        : expression_additive PLUS expression_multiplicative {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_additive_plus;
                $$->typestr = std::string("expression_additive_plus");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_additive_minus
        : expression_additive MINUS expression_multiplicative {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_additive_minus;
                $$->typestr = std::string("expression_additive_minus");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_shift
        : expression_shift_additive {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_shift_left {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_shift_right {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_shift_additive
        : expression_additive {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;
expression_shift_left
        : expression_shift LEFT_OP expression_additive {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_shift_left;
                $$->typestr = std::string("expression_shift_left");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_shift_right
        : expression_shift RIGHT_OP expression_additive {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_shift_right;
                $$->typestr = std::string("expression_shift_right");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;


expression_relational
        : expression_relational_shift {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_relational_lt {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_relational_gt {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_relational_le {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_relational_ge {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

expression_relational_shift
        : expression_shift {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_lt
        : expression_relational LT_OP expression_shift {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_relational_lt;
                $$->typestr = std::string("expression_relational_lt");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_gt
        : expression_relational GT_OP expression_shift {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_relational_gt;
                $$->typestr = std::string("expression_relational_gt");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_le
        : expression_relational LE_OP expression_shift {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_relational_le;
                $$->typestr = std::string("expression_relational_le");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_ge
        : expression_relational GE_OP expression_shift {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_relational_ge;
                $$->typestr = std::string("expression_relational_ge");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_equality
        : expression_relational {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_equality EQ_OP expression_relational {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_equality;
                $$->typestr = std::string("expression_equality");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        | expression_equality NE_OP expression_relational {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_equality;
                $$->typestr = std::string("expression_equality");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

expression_and
        : expression_equality {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_and ANDPERSAND expression_equality {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_and;
                $$->typestr = std::string("expression_and");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

expression_exclusive_or
        : expression_and {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_exclusive_or XOR_OP expression_and {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_exclusive_or;
                $$->typestr = std::string("expression_exclusive_or");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

expression_inclusive_or
        : expression_exclusive_or {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_inclusive_or PIPE expression_exclusive_or {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_inclusive_or;
                $$->typestr = std::string("expression_exclusive_or");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

expression_logical_and
        : expression_inclusive_or {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_and AND_OP expression_inclusive_or {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_logical_and;
                $$->typestr = std::string("expression_logical_and");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

expression_logical_or
        : expression_logical_and {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_or OR_OP expression_logical_and {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_logical_or;
                $$->typestr = std::string("expression_logical_or");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

expression_conditional
        : expression_logical_or {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_or QUESTIONMARK expression COLON expression_conditional {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_conditional;
                $$->typestr = std::string("expression_conditional");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                PRINT_NONTERMINALS($$);
        }
	;

expression_assignment
        : expression_conditional {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | expression_unary operator_assignment expression_assignment {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind::S_expression_assignment;
                $$->typestr = std::string("expression_assignment");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
	;

operator_assignment
        : EQUALS {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| MUL_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| DIV_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| MOD_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| ADD_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| SUB_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| LEFT_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| RIGHT_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| AND_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        } 
	| XOR_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	| OR_ASSIGN {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
	;

expression
        : expression_assignment {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;


type_name
        : TYPEOF PAREN_L expression PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_name;
                $$->typestr = std::string("type_name");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        | TYPE_NAME {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_name;
                $$->typestr = std::string("type_name");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_class_member_declaration_list
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_member_declaration_list;
                $$->typestr = std::string("class_member_declaration_list");
                PRINT_NONTERMINALS($$);
        }
        | class_member_declaration_list {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

class_member_declaration_list
        : class_member_declaration {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_member_declaration_list;
                $$->typestr = std::string("class_member_declaration_list");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | class_member_declaration_list class_member_declaration {
                $$ = $1;
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        ;

class_member_declaration
        : opt_access_modifier type_specifier IDENTIFIER opt_array_length SEMICOLON {
                // Member
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_member_declaration;
                $$->typestr = std::string("class_member_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                // Method
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_member_declaration;
                $$->typestr = std::string("class_member_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier type_specifier PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
          // Constructor
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_member_declaration;
                $$->typestr = std::string("class_member_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier TILDE type_specifier PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
          // Destructor
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_class_member_declaration;
                $$->typestr = std::string("class_member_declaration");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                PRINT_NONTERMINALS($$);
        }
        | class_definition {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | enum_definition {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        | type_definition {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;


type_access_qualifier
        : /**/ {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_access_qualifier;
                $$->typestr = std::string("type_access_qualifier");
                PRINT_NONTERMINALS($$);
        }
        | CONST {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_access_qualifier;
                $$->typestr = std::string("type_access_qualifier");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | VOLATILE {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_access_qualifier;
                $$->typestr = std::string("type_access_qualifier");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        ;

type_specifier_call_args
        : type_specifier {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier_call_args;
                $$->typestr = std::string("type_specifier_call_args");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier_call_args COMMA type_specifier {
                $$ = $1;
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

type_specifier
        : type_access_qualifier type_name {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->typestr = std::string("type_specifier");
                $$->children.push_back($1);
                $$->children.push_back($2);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier PAREN_L type_specifier_call_args PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->typestr = std::string("type_specifier");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier PAREN_L STAR IDENTIFIER PAREN_R PAREN_L opt_function_definition_arg_list PAREN_R {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->typestr = std::string("type_specifier");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                $$->children.push_back($4);
                $$->children.push_back($5);
                $$->children.push_back($6);
                $$->children.push_back($7);
                $$->children.push_back($8);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier STAR type_access_qualifier {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->typestr = std::string("type_specifier-pointer-to");
                $$->children.push_back($1);
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier ANDPERSAND type_access_qualifier {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_type_specifier;
                $$->typestr = std::string("type_specifier-reference-to");
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
                $$->typestr = std::string("argument_expression_list");
                PRINT_NONTERMINALS($$);
        }
        | argument_expression_list {
                $$ = $1;
                PRINT_NONTERMINALS($$);
        }
        ;

argument_expression_list
        : expression {
                $$ = std::make_shared<ASTNode>();
                $$->type = Parser::symbol_kind_type::S_argument_expression_list;
                $$->typestr = std::string("argument_expression_list");
                $$->children.push_back($1);
                PRINT_NONTERMINALS($$);
        }
        | argument_expression_list COMMA expression {
                $$ = $1;
                $$->children.push_back($2);
                $$->children.push_back($3);
                PRINT_NONTERMINALS($$);
        }
        ;

%%

int visibility_from_modifier(ASTNode::ptr node)
{
    int visibility = Namespace::VISIBILITY_PUBLIC;
    if (node) {
      if (node->children.size() == 0) {
        visibility = Namespace::VISIBILITY_PUBLIC;
      }
      else if (node->children.back()->value == "public") {
        visibility = Namespace::VISIBILITY_PUBLIC;
      }
      else if (node->children.back()->value == "protected") {
        visibility = Namespace::VISIBILITY_PROTECTED;
      }
      else if (node->children.back()->value == "private") {
        visibility = Namespace::VISIBILITY_PRIVATE;
      }
    }
    return visibility;
}

void calc::Parser::error(const std::string& msg) {
    printf("Syntax error at line %d : %s\n", lineno, msg.c_str());
}
