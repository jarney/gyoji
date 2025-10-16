%{
/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <iostream>
#include <string>
#include <cmath>
#include <memory>
#include <gyoji.l.hpp>
#include <gyoji-frontend.hpp>
#include <gyoji-misc/jstring.hpp>
  
  using namespace Gyoji::context;
  using namespace Gyoji::frontend::namespaces;
  
%}
 
%require "3.7.4"
%language "C++"
 
%define api.parser.class {YaccParser}
%define api.namespace {Gyoji::frontend::yacc}
%define api.value.type variant
%param {yyscan_t scanner}
%define parse.error verbose

%code provides
{
#define DEBUG_NONTERMINALS 0
#if DEBUG_NONTERMINALS
#define PRINT_NONTERMINALS(s) printf("%s\n", s->get_syntax_node()->get_type().c_str())
#else
#define PRINT_NONTERMINALS(s) /**/
#endif

#define YY_DECL                                                         \
   int yylex(Gyoji::frontend::yacc::YaccParser::semantic_type *yylval, yyscan_t yyscanner)
    YY_DECL;
}

%token INVALID_INPUT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> YYEOF
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> IDENTIFIER
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> NAMESPACE_NAME
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> TYPE_NAME
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> CLASS
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> ENUM

%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> NAMESPACE
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> AS
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> USING
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> TYPEDEF
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> STRUCT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> UNION
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> SIZEOF
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> TYPEOF
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> CAST
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> UNSAFE
 //%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> VAR
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> CONST
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> VOLATILE

%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PUBLIC
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PRIVATE
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PROTECTED

 /* Logical Operations */
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> LOGICAL_NOT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> LOGICAL_AND
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>>  LOGICAL_OR

 /* Control Flow */
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> RETURN
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> IF
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> ELSE
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> WHILE
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> FOR
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> CONTINUE
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> GOTO
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> LABEL
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> BREAK
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> SWITCH
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> CASE
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> DEFAULT

 /* Binary operations */
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PLUS
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> MINUS
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> SLASH
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PERCENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> STAR
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> BANG
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> TILDE
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> ANDPERSAND
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> MUL_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> DIV_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> MOD_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> ADD_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> SUB_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> LEFT_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> RIGHT_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> AND_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> XOR_ASSIGNMENT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> OR_ASSIGNMENT

%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>>  LITERAL_CHAR
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>>  LITERAL_FLOAT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>>  LITERAL_INT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>>  LITERAL_STRING
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>>  LITERAL_BOOL
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>>  LITERAL_NULL

%token NST_COMMENT_MULTILINE
%token NST_COMMENT_SINGLE_LINE
%token NST_WHITESPACE
%token NST_FILE_METADATA

%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> COMPARE_LESS
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> COMPARE_GREATER
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> COMPARE_LESS_EQUAL
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> COMPARE_GREATER_EQUAL
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> COMPARE_NOT_EQUAL
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> COMPARE_EQUAL

%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> XOR_OP
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> AND_OP
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PIPE
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> OR_OP
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> LEFT_OP
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> RIGHT_OP

/* Common punctuation */
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> COMMA
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> INC_OP
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> DEC_OP
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PTR_OP
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> DOT
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PAREN_L
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PAREN_R
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> BRACE_L
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> BRACKET_R
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> BRACKET_L
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> BRACE_R
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> SEMICOLON
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> COLON
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> QUESTIONMARK

%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PREC_FIRST
%token <Gyoji::owned<Gyoji::frontend::tree::Terminal>> PREC_SECOND

%nterm <Gyoji::owned<Gyoji::frontend::tree::TranslationUnit>> translation_unit;

%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatementList>> opt_file_statement_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatementList>> file_statement_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatement>> file_statement;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatementFunctionDeclStart>> function_decl_start;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatementFunctionDefinition>> file_statement_function_definition;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatementFunctionDeclaration>> file_statement_function_declaration;
%nterm <Gyoji::owned<Gyoji::frontend::tree::TypeDefinition>> type_definition;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ClassDefinition>> class_definition;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ClassDeclaration>> class_declaration;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ClassDeclStart>> class_decl_start;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ClassArgumentList>> opt_class_argument_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ClassArgumentList>> class_argument_list;


%nterm <Gyoji::owned<Gyoji::frontend::tree::EnumDefinition>> enum_definition;
%nterm <Gyoji::owned<Gyoji::frontend::tree::EnumDefinitionValueList>> opt_enum_value_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::EnumDefinitionValueList>> enum_value_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::EnumDefinitionValue>> enum_value;

%nterm <Gyoji::owned<Gyoji::frontend::tree::UnsafeModifier>> opt_unsafe
%nterm <Gyoji::owned<Gyoji::frontend::tree::NamespaceDeclaration>> namespace_declaration;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatementNamespace>> file_statement_namespace;
%nterm <Gyoji::owned<Gyoji::frontend::tree::UsingAs>> opt_as;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatementUsing>> file_statement_using;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FileStatementGlobalDefinition>> file_statement_global_definition;
%nterm <Gyoji::owned<Gyoji::frontend::tree::GlobalInitializer>> opt_global_initializer;

%nterm <Gyoji::owned<Gyoji::frontend::tree::GlobalInitializerExpressionPrimary>> global_initializer_expression_primary;
%nterm <Gyoji::owned<Gyoji::frontend::tree::GlobalInitializerAddressofExpressionPrimary>> global_initializer_addressof_expression_primary;
%nterm <Gyoji::owned<Gyoji::frontend::tree::GlobalInitializerStructInitializerList>> global_initializer_struct_initializer_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::GlobalInitializer>> global_initializer;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StructInitializerList>> opt_struct_initializer_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StructInitializerList>> struct_initializer_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StructInitializer>> struct_initializer;

%nterm <Gyoji::owned<Gyoji::frontend::tree::AccessModifier>> opt_access_modifier;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Terminal>> access_modifier;

%nterm <Gyoji::owned<Gyoji::frontend::tree::ScopeBody>> scope_body;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementList>> statement_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Statement>> statement;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementVariableDeclaration>> statement_variable_declaration;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementBlock>> statement_block;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementExpression>> statement_expression;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementIfElse>> statement_ifelse;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementWhile>> statement_while;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementFor>> statement_for;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementSwitch>> statement_switch;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementLabel>> statement_label;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementGoto>> statement_goto;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementBreak>> statement_break;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementContinue>> statement_continue;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementReturn>> statement_return;

%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementSwitchBlock>> statement_switch_block;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementSwitchContent>> statement_switch_content;
%nterm <Gyoji::owned<Gyoji::frontend::tree::StatementSwitchContent>> opt_statement_switch_content;

%nterm <Gyoji::owned<Gyoji::frontend::tree::FunctionDefinitionArgList>> opt_function_definition_arg_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FunctionDefinitionArgList>> function_definition_arg_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::FunctionDefinitionArg>> function_definition_arg

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_primary;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ExpressionPrimaryNested>> expression_primary_nested;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ExpressionPrimaryIdentifier>> expression_primary_identifier;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ExpressionPrimaryLiteralInt>> expression_primary_literal_int;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ExpressionPrimaryLiteralChar>> expression_primary_literal_char;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ExpressionPrimaryLiteralString>> expression_primary_literal_string;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ExpressionPrimaryLiteralFloat>> expression_primary_literal_float;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ExpressionPrimaryLiteralBool>> expression_primary_literal_bool;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ExpressionPrimaryLiteralNull>> expression_primary_literal_null;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_postfix;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_postfix_primary;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_postfix_arrayindex;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_postfix_function_call;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_postfix_dot;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_postfix_arrow;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_postfix_increment;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_postfix_decrement;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_unary;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_unary_increment;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_unary_decrement;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_unary_prefix;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_unary_sizeof_type;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_cast;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_cast_unary;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_cast_cast;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_multiplicative;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_multiplicative_cast;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_multiplicative_multiply;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_multiplicative_divide;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_multiplicative_modulo;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_additive;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_additive_multiplicative;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_additive_plus;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_additive_minus;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_shift;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_shift_additive;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_shift_left;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_shift_right;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_relational;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_relational_shift;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_relational_gt;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_relational_lt;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_relational_le;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_relational_ge;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_equality;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_and;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_exclusive_or;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_inclusive_or;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_logical_and;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_logical_or;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_conditional;
%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression_assignment;

%nterm <Gyoji::owned<Gyoji::frontend::tree::InitializerExpression>> initializer_expression;

%nterm <Gyoji::owned<Gyoji::frontend::tree::TypeSpecifier>> type_specifier;
%nterm <Gyoji::owned<Gyoji::frontend::tree::TypeSpecifierCallArgs>> type_specifier_call_args;
%nterm <Gyoji::owned<Gyoji::frontend::tree::TypeName>> type_name;
%nterm <Gyoji::owned<Gyoji::frontend::tree::AccessQualifier>> type_access_qualifier;

%nterm <Gyoji::owned<Gyoji::frontend::tree::ClassMemberDeclarationList>> opt_class_member_declaration_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ClassMemberDeclarationList>> class_member_declaration_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ClassMemberDeclaration>> class_member_declaration;

%nterm <
    std::pair<
        Gyoji::frontend::tree::ExpressionUnaryPrefix::OperationType,
        Gyoji::owned<Gyoji::frontend::tree::Terminal>
    >
  > operator_unary;
%nterm <
    std::pair<
        Gyoji::frontend::tree::ExpressionBinary::OperationType,
        Gyoji::owned<Gyoji::frontend::tree::Terminal>
    >
  > operator_assignment;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ArgumentExpressionList>> opt_argument_expression_list;
%nterm <Gyoji::owned<Gyoji::frontend::tree::ArgumentExpressionList>> argument_expression_list;

%nterm <Gyoji::owned<Gyoji::frontend::tree::Expression>> expression;

%parse-param {Gyoji::frontend::ParseResult & return_data}

%code
{

} // %code

%%

/*** Rules Section ***/
translation_unit
        : opt_file_statement_list YYEOF {
          $$ = std::make_unique<Gyoji::frontend::tree::TranslationUnit>(std::move($1), std::move($2));
          PRINT_NONTERMINALS($$);
          return_data.set_translation_unit(std::move($$));
        }
        ;

opt_file_statement_list 
        : /**/ YYEOF {
          $$ = std::make_unique<Gyoji::frontend::tree::FileStatementList>(std::move($1));
          PRINT_NONTERMINALS($$);
        }
        | file_statement_list {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        ;

file_statement_list 
        : file_statement {
          $$ = std::make_unique<Gyoji::frontend::tree::FileStatementList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
          $$->add_statement(std::move($1));
          PRINT_NONTERMINALS($$);
        }
        | file_statement_list file_statement {
          $$ = std::move($1);
          $$->add_statement(std::move($2));
          PRINT_NONTERMINALS($$);
        }
        ;

file_statement
        : file_statement_function_definition {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | file_statement_function_declaration {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | file_statement_global_definition {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | class_declaration {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                            std::move($1),
                                                                            sn
                                                                            );
                PRINT_NONTERMINALS($$);
        }
        | class_definition {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | enum_definition {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | type_definition {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | file_statement_namespace {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | file_statement_using {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_global_definition
        : opt_access_modifier opt_unsafe type_specifier IDENTIFIER opt_global_initializer SEMICOLON {
	        std::string global_name = $4->get_value();
		NS2Entity *ns2_entity = return_data.identifier_get_or_create($4->get_value(), true, $4->get_source_ref());
		$4->set_ns2_entity(ns2_entity);
	
	        $$ = std::make_unique<Gyoji::frontend::tree::FileStatementGlobalDefinition>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3),
                                                                                      std::move($4),
                                                                                      std::move($5),
                                                                                      std::move($6)
                                                                                      );
          PRINT_NONTERMINALS($$);
        }
        ;

opt_global_initializer
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::GlobalInitializer>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | global_initializer {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

global_initializer
        : global_initializer_expression_primary {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::GlobalInitializer>(std::move($1), sn);
        }
        | global_initializer_addressof_expression_primary {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::GlobalInitializer>(std::move($1), sn);
        }
        | global_initializer_struct_initializer_list {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::GlobalInitializer>(std::move($1), sn);
        }
        ;

global_initializer_expression_primary
        : ASSIGNMENT expression_primary {
          $$ = std::make_unique<Gyoji::frontend::tree::GlobalInitializerExpressionPrimary>(std::move($1),
                                                                                              std::move($2));
          PRINT_NONTERMINALS($$);
        }
        ;

global_initializer_addressof_expression_primary
        : ASSIGNMENT ANDPERSAND expression_primary {
          $$ = std::make_unique<Gyoji::frontend::tree::GlobalInitializerAddressofExpressionPrimary>(std::move($1),
                                                                                                       std::move($2),
                                                                                                       std::move($3)
                                                                                                       );
          PRINT_NONTERMINALS($$);
        }
        ;

global_initializer_struct_initializer_list
        : ASSIGNMENT BRACE_L opt_struct_initializer_list BRACE_R {
          $$ = std::make_unique<Gyoji::frontend::tree::GlobalInitializerStructInitializerList>(std::move($1),
                                                                                                  std::move($2),
                                                                                                  std::move($3),
                                                                                                  std::move($4)
                                                                                                  );
          PRINT_NONTERMINALS($$);
        }
        ;


opt_struct_initializer_list
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::StructInitializerList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | struct_initializer_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

struct_initializer_list
        : struct_initializer {
                $$ = std::make_unique<Gyoji::frontend::tree::StructInitializerList>($1->get_source_ref());
                $$->add_initializer(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | struct_initializer_list struct_initializer {
                $$ = std::move($1);
                $$->add_initializer(std::move($2));
                PRINT_NONTERMINALS($$);
        }
        ;

struct_initializer
        : DOT IDENTIFIER global_initializer SEMICOLON {
	        $2->set_identifier_type(Gyoji::frontend::tree::Terminal::IDENTIFIER_LOCAL_SCOPE);
		
                $$ = std::make_unique<Gyoji::frontend::tree::StructInitializer>(
                                                                                   std::move($1),
                                                                                   std::move($2),
                                                                                   std::move($3),
                                                                                   std::move($4)
                                                                                   );
                PRINT_NONTERMINALS($$);
        }
        ;

opt_access_modifier
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::AccessModifier>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | access_modifier {
                $$ = std::make_unique<Gyoji::frontend::tree::AccessModifier>(
                                                                             std::move($1)
                                                                             );
                PRINT_NONTERMINALS($$);
        }
        ;

access_modifier
        : PUBLIC {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | PRIVATE {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | PROTECTED {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

namespace_declaration
        : opt_access_modifier NAMESPACE IDENTIFIER {
		Gyoji::frontend::namespaces::NS2Entity *ns = return_data.namespace_get_or_create($3->get_value(), $3->get_source_ref());
		$3->set_ns2_entity(ns);
		return_data.ns2_context->namespace_push(ns);
		
                $$ = std::make_unique<Gyoji::frontend::tree::NamespaceDeclaration>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );

                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier NAMESPACE NAMESPACE_NAME {
		Gyoji::frontend::namespaces::NS2Entity *ns = return_data.ns2_context->namespace_find($3->get_value());
		return_data.ns2_context->namespace_push(ns);
		
                $$ = std::make_unique<Gyoji::frontend::tree::NamespaceDeclaration>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_namespace
        : namespace_declaration BRACE_L opt_file_statement_list BRACE_R SEMICOLON {
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatementNamespace>(
                                                                                        std::move($1),
                                                                                        std::move($2),
                                                                                        std::move($3),
                                                                                        std::move($4),
                                                                                        std::move($5)
                                                                                        );
		return_data.ns2_context->namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

opt_as
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::UsingAs>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | AS IDENTIFIER {
                $$ = std::make_unique<Gyoji::frontend::tree::UsingAs>(
                                                                std::move($1),
                                                                std::move($2)
                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_using
        : opt_access_modifier USING NAMESPACE NAMESPACE_NAME opt_as SEMICOLON {
                std::string namespace_name = $4->get_value();
                std::string as_name = $5->get_using_name();
		NS2Entity *entity = return_data.ns2_context->namespace_find(namespace_name);
		if (entity == nullptr) {
		    auto error = std::make_unique<Gyoji::context::Error>(std::string("Invalid identifier") + namespace_name + std::string("."));
		    error->add_message($4->get_source_ref(), std::string("") + namespace_name + std::string(" is not a namespace."));
		    return_data.get_compiler_context().get_errors().add_error(std::move(error));
		    return -1;
		}
		return_data.ns2_context->namespace_using(as_name, entity);

                $$ = std::make_unique<Gyoji::frontend::tree::FileStatementUsing>(
                                                                                    std::move($1),
                                                                                    std::move($2),
                                                                                    std::move($3),
                                                                                    std::move($4),
                                                                                    std::move($5),
                                                                                    std::move($6)
                                                                                    );
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier USING NAMESPACE TYPE_NAME opt_as SEMICOLON {
                std::string namespace_name = $4->get_value();
                std::string as_name = $5->get_using_name();

		NS2Entity *entity = return_data.ns2_context->namespace_find(namespace_name);
		if (entity == nullptr) {
		    auto error = std::make_unique<Gyoji::context::Error>(std::string("Invalid identifier") + namespace_name + std::string("."));
		    error->add_message($4->get_source_ref(), std::string("") + namespace_name + std::string(" is not a namespace."));
		    return_data.get_compiler_context().get_errors().add_error(std::move(error));
		    return -1;
		}
		return_data.ns2_context->namespace_using(as_name, entity);
		
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatementUsing>(
                                                                                    std::move($1),
                                                                                    std::move($2),
                                                                                    std::move($3),
                                                                                    std::move($4),
                                                                                    std::move($5),
                                                                                    std::move($6)
                                                                                    );
                PRINT_NONTERMINALS($$);
        }
        ;

// If this is the first time we see the declaration,
// it will be an identifier.  Once the class has been
// seen before (forward-declared), it will then become
// a type instead of an identifier.
class_decl_start
        : opt_access_modifier CLASS IDENTIFIER opt_class_argument_list {
		NS2Entity *ns2_entity = return_data.class_get_or_create($3->get_value(), $3->get_source_ref());
		$3->set_ns2_entity(ns2_entity);
		return_data.ns2_context->namespace_push(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassDeclStart>(
                                                                         std::move($1),
                                                                         std::move($2),
                                                                         std::move($3),
                                                                         std::move($4),
                                                                         true // is_identifier
                                                                         );

		
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier CLASS TYPE_NAME opt_class_argument_list {
	        return_data.ns2_context->namespace_push($3->get_ns2_entity());
                $$ = std::make_unique<Gyoji::frontend::tree::ClassDeclStart>(
                                                                         std::move($1),
                                                                         std::move($2),
                                                                         std::move($3),
                                                                         std::move($4),
                                                                         false // is_identifier
                                                                         );
        }
        ;

opt_class_argument_list
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::ClassArgumentList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | PAREN_L class_argument_list PAREN_R {
                $$ = std::move($2);
                $$->add_parens(std::move($1), std::move($3));
                PRINT_NONTERMINALS($$);
        }
        ;

// At this stage, the arguments
// are identifiers.  Once they are
// recognized, they are turned into
// types scoped private in the class.
class_argument_list
        : IDENTIFIER {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($1->get_value(), true, $1->get_source_ref());
		$1->set_ns2_entity(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassArgumentList>(
                                                                                  std::move($1)
                                                                                  );
                PRINT_NONTERMINALS($$);
        }
        | class_argument_list COMMA IDENTIFIER {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($3->get_value(), true, $3->get_source_ref());
		$3->set_ns2_entity(ns2_entity);
                $$ = std::move($1);
                $$->add_argument(std::move($2), std::move($3));
                PRINT_NONTERMINALS($$);
        }
        ;

// Forwar-declaration of a class.
class_declaration
        : class_decl_start SEMICOLON {
                $$ = std::make_unique<Gyoji::frontend::tree::ClassDeclaration>(
		    std::move($1),
		    std::move($2)
		    );
                return_data.ns2_context->namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

class_definition
        : class_decl_start BRACE_L opt_class_member_declaration_list BRACE_R SEMICOLON {
                $$ = std::make_unique<Gyoji::frontend::tree::ClassDefinition>(
                                                                                 std::move($1),
                                                                                 std::move($2),
                                                                                 std::move($3),
                                                                                 std::move($4),
                                                                                 std::move($5)
                                                                                 );
                return_data.ns2_context->namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

type_definition
        : opt_access_modifier TYPEDEF type_specifier IDENTIFIER SEMICOLON {
                Gyoji::frontend::namespaces::NS2Entity *ns2_entity = return_data.type_get_or_create($4->get_value(), $4->get_source_ref());
                $4->set_ns2_entity(ns2_entity);
		fprintf(stderr, "Defined type %s\n", $4->get_fully_qualified_name().c_str());
                $$ = std::make_unique<Gyoji::frontend::tree::TypeDefinition>(
                                                                               std::move($1),
                                                                               std::move($2),
                                                                               std::move($3),
                                                                               std::move($4),
                                                                               std::move($5)
                                                                               );

		
                PRINT_NONTERMINALS($$);
        }
        ;

enum_definition
        : opt_access_modifier ENUM TYPE_NAME IDENTIFIER BRACE_L opt_enum_value_list BRACE_R SEMICOLON {
                Gyoji::frontend::namespaces::NS2Entity *ns2_entity = return_data.type_get_or_create($4->get_value(), $4->get_source_ref());
		$4->set_ns2_entity(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::EnumDefinition>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3),
                                                                                std::move($4),
                                                                                std::move($5),
                                                                                std::move($6),
                                                                                std::move($7),
                                                                                std::move($8)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

opt_enum_value_list
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::EnumDefinitionValueList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | enum_value_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

enum_value_list
        : enum_value {
                $$ = std::make_unique<Gyoji::frontend::tree::EnumDefinitionValueList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                $$->add_value(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | enum_value_list enum_value {
                $$ = std::move($1);
                $$->add_value(std::move($2));
                PRINT_NONTERMINALS($$);
        }
        ;

enum_value
        : IDENTIFIER ASSIGNMENT expression_primary SEMICOLON {
                Gyoji::frontend::namespaces::NS2Entity *ns2_entity = return_data.type_get_or_create($1->get_value(), $1->get_source_ref());
		$1->set_ns2_entity(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::EnumDefinitionValue>(
                                                                                    std::move($1),
                                                                                    std::move($2),
                                                                                    std::move($3),
                                                                                    std::move($4)
                                                                                    );
                PRINT_NONTERMINALS($$);
        }
        ;
  
opt_unsafe
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::UnsafeModifier>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | UNSAFE {
                $$ = std::make_unique<Gyoji::frontend::tree::UnsafeModifier>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_function_declaration
        : function_decl_start PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
	        // This is the point at which we need to fully-qualify our symbol name
	        // because it may be in a namespace and we need to find it unambiguously
	        // in a possibly large namespace.  We don't want the MIR or code-gen layers
	        // to have to care about how we name functions.
	        // In future, we may support name mangling, but again, that's not the concern of the
	        // back-end layers.
                $$ = std::make_unique<Gyoji::frontend::tree::FileStatementFunctionDeclaration>(
                                                                                                 std::move($1),
                                                                                                 std::move($2),
                                                                                                 std::move($3),
                                                                                                 std::move($4),
                                                                                                 std::move($5)
                                                                                                 );
		return_data.ns2_context->namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

function_decl_start
        : opt_access_modifier opt_unsafe type_specifier IDENTIFIER {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($4->get_value(), true, $4->get_source_ref());
		if (ns2_entity == nullptr) {
		    return -1;
		}
		$4->set_ns2_entity(ns2_entity);
		return_data.ns2_context->namespace_push(ns2_entity);
		$$ = std::make_unique<Gyoji::frontend::tree::FileStatementFunctionDeclStart>(
                        std::move($1),
                        std::move($2),
                        std::move($3),
                        std::move($4)
                        );
        }
        ;

file_statement_function_definition
        : function_decl_start PAREN_L opt_function_definition_arg_list PAREN_R scope_body {
	        // This is the point at which we need to fully-qualify our symbol name
	        // because it may be in a namespace and we need to find it unambiguously
	        // in a possibly large namespace.  We don't want the MIR or code-gen layers
	        // to have to care about how we name functions.
	        // In future, we may support name mangling, but again, that's not the concern of the
	        // back-end layers.

		// For a function definition, we may want to also 'push' the namespace
		// resolution context for a class if the identifier is declared inside
		// a class' namespace. but that needs to happen before we actually push the scope
		// and function identifiers.

                $$ = std::make_unique<Gyoji::frontend::tree::FileStatementFunctionDefinition>(
                                                                                                std::move($1),
                                                                                                std::move($2),
                                                                                                std::move($3),
                                                                                                std::move($4),
                                                                                                std::move($5)
                                                                                                );
		return_data.ns2_context->namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

opt_function_definition_arg_list
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::FunctionDefinitionArgList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | function_definition_arg_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

function_definition_arg_list
        : function_definition_arg {
                $$ = std::make_unique<Gyoji::frontend::tree::FunctionDefinitionArgList>($1->get_source_ref());
                $$->add_argument(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | function_definition_arg_list COMMA function_definition_arg {
                $$ = std::move($1);
                $$->add_comma(std::move($2));
                $$->add_argument(std::move($3));
                PRINT_NONTERMINALS($$);
        }
        ;
function_definition_arg
        : type_specifier IDENTIFIER {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($2->get_value(), true, $2->get_source_ref());
	        $2->set_ns2_entity(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::FunctionDefinitionArg>(
		    std::move($1),
		    std::move($2)
		    );
                PRINT_NONTERMINALS($$);
        }
        ;

scope_body
        : BRACE_L statement_list BRACE_R {
                $$ = std::make_unique<Gyoji::frontend::tree::ScopeBody>(
                                                                           std::move($1),
                                                                           std::move($2),
                                                                           std::move($3)
                                                                           );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_list
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | statement_list statement {
                $$ = std::move($1);
                $$->add_statement(std::move($2));
                PRINT_NONTERMINALS($$);
        }
        ;

statement
        : statement_variable_declaration {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_block {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_expression {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_ifelse {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_while {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_for {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_switch {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_label {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_goto {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_break {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_continue {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | statement_return {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Statement>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

initializer_expression
: /**/ {
    $$ = std::make_unique<Gyoji::frontend::tree::InitializerExpression>(return_data.compiler_context.get_token_stream().get_current_source_ref());
    PRINT_NONTERMINALS($$);
}
| ASSIGNMENT expression {
    $$ = std::make_unique<Gyoji::frontend::tree::InitializerExpression>(
	std::move($1),
	std::move($2)
	);
    PRINT_NONTERMINALS($$);
}
;

statement_variable_declaration
        : type_specifier IDENTIFIER initializer_expression SEMICOLON {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($2->get_value(), true, $2->get_source_ref());
	        $2->set_ns2_entity(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::StatementVariableDeclaration>(
		    std::move($1),
		    std::move($2),
		    std::move($3),
		    std::move($4)
		    );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_block
        : opt_unsafe scope_body {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementBlock>(
                                                                                std::move($1),
                                                                                std::move($2)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_expression
        : expression SEMICOLON {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementExpression>(
                                                                                std::move($1),
                                                                                std::move($2)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;
statement_goto
        : GOTO IDENTIFIER SEMICOLON {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($2->get_value(), true, $2->get_source_ref());
		$2->set_ns2_entity(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::StatementGoto>(
                                                                               std::move($1),
                                                                               std::move($2),
                                                                               std::move($3)
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        ;
statement_break
        : BREAK SEMICOLON {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementBreak>(
                                                                                std::move($1),
                                                                                std::move($2)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;
statement_continue
        : CONTINUE SEMICOLON {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementContinue>(
                                                                                std::move($1),
                                                                                std::move($2)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;
statement_label
        : LABEL IDENTIFIER COLON {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($2->get_value(), true, $2->get_source_ref());
		$2->set_ns2_entity(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::StatementLabel>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_return
        : RETURN expression SEMICOLON {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementReturn>(
                                                                                 std::move($1),
                                                                                 std::move($2),
                                                                                 std::move($3)
                                                                                 );
                PRINT_NONTERMINALS($$);
        }
        | RETURN SEMICOLON {
	        $$ = std::make_unique<Gyoji::frontend::tree::StatementReturn>(
		    std::move($1),
                    std::move($2)
		);
                PRINT_NONTERMINALS($$);
        }
	
        ; 

statement_ifelse
        : IF PAREN_L expression PAREN_R scope_body {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementIfElse>(
                                                                                 std::move($1),
                                                                                 std::move($2),
                                                                                 std::move($3),
                                                                                 std::move($4),
                                                                                 std::move($5)
                                                                                 );
                PRINT_NONTERMINALS($$);
        }
        | IF PAREN_L expression PAREN_R scope_body ELSE statement_ifelse {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementIfElse>(
                                                                                 std::move($1),
                                                                                 std::move($2),
                                                                                 std::move($3),
                                                                                 std::move($4),
                                                                                 std::move($5),
                                                                                 std::move($6),
                                                                                 std::move($7)
                                                                                 );
                PRINT_NONTERMINALS($$);
        }
        | IF PAREN_L expression PAREN_R scope_body ELSE scope_body {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementIfElse>(
                                                                                 std::move($1),
                                                                                 std::move($2),
                                                                                 std::move($3),
                                                                                 std::move($4),
                                                                                 std::move($5),
                                                                                 std::move($6),
                                                                                 std::move($7)
                                                                                 );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_while
        : WHILE PAREN_L expression PAREN_R scope_body {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementWhile>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3),
                                                                                std::move($4),
                                                                                std::move($5)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_for
        : FOR PAREN_L expression SEMICOLON expression SEMICOLON expression PAREN_R scope_body {
                // This variation is just a plain expression.
                $$ = std::make_unique<Gyoji::frontend::tree::StatementFor>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3),
                                                                                std::move($4),
                                                                                std::move($5),
                                                                                std::move($6),
                                                                                std::move($7),
                                                                                std::move($8),
                                                                                std::move($9)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        | FOR PAREN_L type_specifier IDENTIFIER ASSIGNMENT expression SEMICOLON expression SEMICOLON expression PAREN_R scope_body {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($4->get_value(), true, $4->get_source_ref());
		$2->set_ns2_entity(ns2_entity);
                // This variation is a declaration and assignment
                $$ = std::make_unique<Gyoji::frontend::tree::StatementFor>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3),
                                                                                std::move($4),
                                                                                std::move($5),
                                                                                std::move($6),
                                                                                std::move($7),
                                                                                std::move($8),
                                                                                std::move($9),
                                                                                std::move($10),
                                                                                std::move($11),
                                                                                std::move($12)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_switch
        : SWITCH PAREN_L expression PAREN_R BRACE_L opt_statement_switch_content BRACE_R {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementSwitch>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3),
                                                                                std::move($4),
                                                                                std::move($5),
                                                                                std::move($6),
                                                                                std::move($7)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

opt_statement_switch_content
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementSwitchContent>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | statement_switch_content {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_switch_content
        : statement_switch_block {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementSwitchContent>($1->get_source_ref());
                $$->add_block(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_switch_content statement_switch_block {
                $$ = std::move($1);
                $$->add_block(std::move($2));
                PRINT_NONTERMINALS($$);
        }
        ;

statement_switch_block
        : DEFAULT COLON scope_body {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementSwitchBlock>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                PRINT_NONTERMINALS($$);
        }
        | CASE expression COLON scope_body {
                $$ = std::make_unique<Gyoji::frontend::tree::StatementSwitchBlock>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3),
                                                                                      std::move($4)
                                                                                      );
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary
        : expression_primary_identifier {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_nested {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_int {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_char {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_string {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_float {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_bool {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_null {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_identifier
        : IDENTIFIER {
                NS2Entity *ns2_entity = return_data.identifier_get_or_create($1->get_value(), true, $1->get_source_ref());
		$1->set_ns2_entity(ns2_entity);
                $$ = std::make_unique<Gyoji::frontend::tree::ExpressionPrimaryIdentifier>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_literal_int
        : LITERAL_INT {
                $$ = std::make_unique<Gyoji::frontend::tree::ExpressionPrimaryLiteralInt>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_char
        : LITERAL_CHAR {
                $$ = std::make_unique<Gyoji::frontend::tree::ExpressionPrimaryLiteralChar>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_string
        : LITERAL_STRING {
                $$ = std::make_unique<Gyoji::frontend::tree::ExpressionPrimaryLiteralString>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_string LITERAL_STRING {
    	        $$ = std::move($1);
        	$$->add_string(std::move($2));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_literal_float
        : LITERAL_FLOAT {
                $$ = std::make_unique<Gyoji::frontend::tree::ExpressionPrimaryLiteralFloat>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_literal_bool
        : LITERAL_BOOL {
                $$ = std::make_unique<Gyoji::frontend::tree::ExpressionPrimaryLiteralBool>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_literal_null
        : LITERAL_NULL {
                $$ = std::make_unique<Gyoji::frontend::tree::ExpressionPrimaryLiteralNull>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_nested
        : PAREN_L expression PAREN_R {
                $$ = std::make_unique<Gyoji::frontend::tree::ExpressionPrimaryNested>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                PRINT_NONTERMINALS($$);
        }
	;

expression_postfix
        : expression_postfix_primary {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_arrayindex {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_function_call {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_dot {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_arrow {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_increment {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_postfix_decrement {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_primary
        : expression_primary {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_arrayindex
        : expression_postfix BRACKET_L expression BRACKET_R {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionPostfixArrayIndex>(
                                                                                             std::move($1),
                                                                                             std::move($2),
                                                                                             std::move($3),
                                                                                             std::move($4)
                                                                                             );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_function_call
        : expression_postfix PAREN_L opt_argument_expression_list PAREN_R {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionPostfixFunctionCall>(
                                                                                               std::move($1),
                                                                                               std::move($2),
                                                                                               std::move($3),
                                                                                               std::move($4)
                                                                                               );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_dot
        : expression_postfix DOT IDENTIFIER {
	        $3->set_identifier_type(Gyoji::frontend::tree::Terminal::IDENTIFIER_LOCAL_SCOPE);
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionPostfixDot>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_arrow
        : expression_postfix PTR_OP IDENTIFIER {
	        $3->set_identifier_type(Gyoji::frontend::tree::Terminal::IDENTIFIER_LOCAL_SCOPE);
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionPostfixArrow>(
                                                                                        std::move($1),
                                                                                        std::move($2),
                                                                                        std::move($3)
                                                                                        );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_increment
        : expression_postfix INC_OP {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionPostfixIncDec>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         Gyoji::frontend::tree::ExpressionPostfixIncDec::INCREMENT
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_decrement
        : expression_postfix DEC_OP {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionPostfixIncDec>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         Gyoji::frontend::tree::ExpressionPostfixIncDec::DECREMENT
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

expression_unary
        : expression_postfix {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_unary_increment {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_unary_decrement {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_unary_prefix {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_unary_sizeof_type {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_unary_increment
        : INC_OP expression_unary {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionUnaryPrefix>(
                                                                                       std::move($1),
                                                                                       std::move($2),
                                                                                       Gyoji::frontend::tree::ExpressionUnaryPrefix::INCREMENT
                                                                                       );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_decrement
        : DEC_OP expression_unary {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionUnaryPrefix>(
                                                                                       std::move($1),
                                                                                       std::move($2),
                                                                                       Gyoji::frontend::tree::ExpressionUnaryPrefix::DECREMENT
                                                                                       );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_prefix
        : operator_unary expression_cast {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionUnaryPrefix>(
                                                                                       std::move($1.second),
                                                                                       std::move($2),
                                                                                       $1.first
                                                                                       );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_sizeof_type
        : SIZEOF PAREN_L type_specifier PAREN_R {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionUnarySizeofType>(
                                                                                       std::move($1),
                                                                                       std::move($2),
                                                                                       std::move($3),
                                                                                       std::move($4)
                                                                                       );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;



operator_unary
        : ANDPERSAND {
                $$ = std::pair(
                         Gyoji::frontend::tree::ExpressionUnaryPrefix::ADDRESSOF,
                         std::move($1)
                         );
                PRINT_NONTERMINALS(($$.second));
        }
        | STAR {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionUnaryPrefix::DEREFERENCE,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
        | PLUS {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionUnaryPrefix::PLUS,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
        | MINUS {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionUnaryPrefix::MINUS,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
        | TILDE {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionUnaryPrefix::BITWISE_NOT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
        | BANG {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionUnaryPrefix::LOGICAL_NOT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	;

expression_cast
        : expression_cast_unary {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_cast_cast {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_cast_unary
        : expression_unary {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_cast_cast
        : CAST PAREN_L type_specifier COMMA expression PAREN_R {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionCast>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3),
                                                                                std::move($4),
                                                                                std::move($5),
                                                                                std::move($6)
                                                                                );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;


expression_multiplicative
        : expression_multiplicative_cast {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_multiplicative_multiply {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_multiplicative_divide {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_multiplicative_modulo {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_multiplicative_cast
        : expression_cast {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_multiplicative_multiply
        : expression_multiplicative STAR expression_cast {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::MULTIPLY
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_multiplicative_divide
        : expression_multiplicative SLASH expression_cast {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::DIVIDE
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_multiplicative_modulo
        : expression_multiplicative PERCENT expression_cast {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::MODULO
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_additive
        : expression_additive_multiplicative {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_additive_plus {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_additive_minus {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_additive_multiplicative
        : expression_multiplicative {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_additive_plus
        : expression_additive PLUS expression_multiplicative {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::ADD
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_additive_minus
        : expression_additive MINUS expression_multiplicative {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::SUBTRACT
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_shift
        : expression_shift_additive {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_shift_left {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_shift_right {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_shift_additive
        : expression_additive {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_shift_left
        : expression_shift LEFT_OP expression_additive {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::SHIFT_LEFT
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_shift_right
        : expression_shift RIGHT_OP expression_additive {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::SHIFT_RIGHT
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;


expression_relational
        : expression_relational_shift {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        | expression_relational_lt {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        | expression_relational_gt {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        | expression_relational_le {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        | expression_relational_ge {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        ;

expression_relational_shift
        : expression_shift {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_lt
        : expression_relational COMPARE_LESS expression_shift {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::COMPARE_LESS
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_gt
        : expression_relational COMPARE_GREATER expression_shift {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::COMPARE_GREATER
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_le
        : expression_relational COMPARE_LESS_EQUAL expression_shift {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::COMPARE_LESS_EQUAL
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_ge
        : expression_relational COMPARE_GREATER_EQUAL expression_shift {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::COMPARE_GREATER_EQUAL
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

expression_equality
        : expression_relational {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        | expression_equality COMPARE_EQUAL expression_relational {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::COMPARE_EQUAL
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | expression_equality COMPARE_NOT_EQUAL expression_relational {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::COMPARE_NOT_EQUAL
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

expression_and
        : expression_equality {
          $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_and ANDPERSAND expression_equality {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::BITWISE_AND
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

expression_exclusive_or
        : expression_and {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        | expression_exclusive_or XOR_OP expression_and {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::BITWISE_XOR
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

expression_inclusive_or
        : expression_exclusive_or {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_inclusive_or PIPE expression_exclusive_or {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::BITWISE_OR
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

expression_logical_and
        : expression_inclusive_or {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_and ANDPERSAND ANDPERSAND expression_inclusive_or {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         std::move($4),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::LOGICAL_AND
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

expression_logical_or
        : expression_logical_and {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_or OR_OP expression_logical_and {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         Gyoji::frontend::tree::ExpressionBinary::LOGICAL_OR
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

expression_conditional
        : expression_logical_or {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_or QUESTIONMARK expression COLON expression_conditional {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionTrinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         std::move($4),
                                                                                         std::move($5)
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

expression_assignment
        : expression_conditional {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_unary operator_assignment expression_assignment {
                auto expr = std::make_unique<Gyoji::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2.second),
                                                                                         std::move($3),
                                                                                         $2.first
                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::Expression>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
	;

operator_assignment
        : ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| MUL_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::MUL_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| DIV_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::DIV_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| MOD_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::MOD_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| ADD_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::ADD_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| SUB_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::SUB_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| LEFT_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::LEFT_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| RIGHT_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::RIGHT_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| AND_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::AND_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        } 
	| XOR_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::XOR_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	| OR_ASSIGNMENT {
                $$ = std::pair(
                               Gyoji::frontend::tree::ExpressionBinary::OR_ASSIGNMENT,
                               std::move($1)
                               );
                PRINT_NONTERMINALS(($$.second));
        }
	;

expression
        : expression_assignment {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;


type_name
        : TYPEOF PAREN_L expression PAREN_R {
                $$ = std::make_unique<Gyoji::frontend::tree::TypeName>(
                                                                          std::move($1),
                                                                          std::move($2),
                                                                          std::move($3),
                                                                          std::move($4)
                                                                          );
                PRINT_NONTERMINALS($$);
        }
        | TYPE_NAME {
                $$ = std::make_unique<Gyoji::frontend::tree::TypeName>(
                                                                          std::move($1)
                                                                          );
                PRINT_NONTERMINALS($$);
        }
        ;

opt_class_member_declaration_list
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclarationList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | class_member_declaration_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

class_member_declaration_list
        : class_member_declaration {
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclarationList>($1->get_source_ref());
                $$->add_member(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | class_member_declaration_list class_member_declaration {
                $$ = std::move($1);
                $$->add_member(std::move($2));
                PRINT_NONTERMINALS($$);
        }
        ;

class_member_declaration
        : opt_access_modifier type_specifier IDENTIFIER SEMICOLON {
                // Member Variable
	        $3->set_identifier_type(Gyoji::frontend::tree::Terminal::IDENTIFIER_LOCAL_SCOPE);
		NS2Entity *entity = return_data.identifier_get_or_create($3->get_value(), false, $3->get_source_ref());
		$3->set_ns2_entity(entity);
                auto expr = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclarationVariable>(
                                                                                                       std::move($1),
                                                                                                       std::move($2),
                                                                                                       std::move($3),
                                                                                                       std::move($4)
                                                                                                       );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclaration>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                // Method
	        $3->set_identifier_type(Gyoji::frontend::tree::Terminal::IDENTIFIER_LOCAL_SCOPE);
		NS2Entity *entity = return_data.identifier_get_or_create($3->get_value(), false, $3->get_source_ref());
		$3->set_ns2_entity(entity);
                auto expr = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclarationMethod>(
                                                                                                     std::move($1),
                                                                                                     std::move($2),
                                                                                                     std::move($3),
                                                                                                     std::move($4),
                                                                                                     std::move($5),
                                                                                                     std::move($6),
                                                                                                     std::move($7)
                                                                                                     );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclaration>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier type_specifier PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                // Constructor
                auto expr = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclarationConstructor>(
                                                                                                     std::move($1),
                                                                                                     std::move($2),
                                                                                                     std::move($3),
                                                                                                     std::move($4),
                                                                                                     std::move($5),
                                                                                                     std::move($6)
                                                                                                     );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclaration>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier TILDE type_specifier PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                // Destructor
                auto expr = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclarationDestructor>(
                                                                                                         std::move($1),
                                                                                                         std::move($2),
                                                                                                         std::move($3),
                                                                                                         std::move($4),
                                                                                                         std::move($5),
                                                                                                         std::move($6),
                                                                                                         std::move($7)
                                                                                                         );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclaration>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | class_declaration {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclaration>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | class_definition {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclaration>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | enum_definition {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclaration>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_definition {
                const Gyoji::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<Gyoji::frontend::tree::ClassMemberDeclaration>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        ;


type_access_qualifier
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::AccessQualifier>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | CONST {
                $$ = std::make_unique<Gyoji::frontend::tree::AccessQualifier>(
                                                                              std::move($1)
                                                                              );
                PRINT_NONTERMINALS($$);
        }
        | VOLATILE {
                $$ = std::make_unique<Gyoji::frontend::tree::AccessQualifier>(
                                                                                 std::move($1)
                                                                                 );
                PRINT_NONTERMINALS($$);
        }
        ;

type_specifier_call_args
        : type_specifier {
                $$ = std::make_unique<Gyoji::frontend::tree::TypeSpecifierCallArgs>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                $$->add_argument(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | type_specifier_call_args COMMA type_specifier {
                $$ = std::move($1);
                $$->add_argument(std::move($2), std::move($3));
                PRINT_NONTERMINALS($$);
        }
        ;

type_specifier
        : type_access_qualifier type_name {
                auto expr = std::make_unique<Gyoji::frontend::tree::TypeSpecifierSimple>(
                                                                                      std::move($1),
                                                                                      std::move($2)
                                                                                      );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier BRACKET_L LITERAL_INT BRACKET_R {
                auto expr = std::make_unique<Gyoji::frontend::tree::TypeSpecifierArray>(
	                std::move($1),
			std::move($2),
			std::move($3),
			std::move($4)
		);
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier PAREN_L type_specifier_call_args PAREN_R {
                auto expr = std::make_unique<Gyoji::frontend::tree::TypeSpecifierTemplate>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3),
                                                                                      std::move($4)
                                                                                      );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier PAREN_L STAR IDENTIFIER PAREN_R PAREN_L opt_function_definition_arg_list PAREN_R {
                NS2Entity *entity = return_data.type_get_or_create($4->get_value(), $4->get_source_ref());
		$4->set_ns2_entity(entity);
                auto expr = std::make_unique<Gyoji::frontend::tree::TypeSpecifierFunctionPointer>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3),
                                                                                      std::move($4),
                                                                                      std::move($5),
                                                                                      std::move($6),
                                                                                      std::move($7),
                                                                                      std::move($8)
                                                                                      );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier STAR type_access_qualifier {
                auto expr = std::make_unique<Gyoji::frontend::tree::TypeSpecifierPointerTo>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier ANDPERSAND type_access_qualifier {
                auto expr = std::make_unique<Gyoji::frontend::tree::TypeSpecifierReferenceTo>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                const Gyoji::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<Gyoji::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_argument_expression_list
        : /**/ {
                $$ = std::make_unique<Gyoji::frontend::tree::ArgumentExpressionList>(return_data.compiler_context.get_token_stream().get_current_source_ref());
                PRINT_NONTERMINALS($$);
        }
        | argument_expression_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

argument_expression_list
        : expression {
                $$ = std::make_unique<Gyoji::frontend::tree::ArgumentExpressionList>($1->get_source_ref());
                $$->add_argument(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | argument_expression_list COMMA expression {
                $$ = std::move($1);
                $$->add_argument(std::move($2), std::move($3));
                PRINT_NONTERMINALS($$);
        }
        ;

%%

void Gyoji::frontend::yacc::YaccParser::error(const std::string& msg) {
    LexContext *lex_context = (LexContext*)yyget_extra(scanner);

    const SourceReference & src_ref = lex_context->compiler_context.get_token_stream().get_current_source_ref();

    // We don't directly print an error here, we report it
    // to the error reporting system so that it can
    // write error information when it needs to, presumably
    // after collecting possibly multiple errors

    // We want to consume more context from the next few
    // lines so we can produce a good syntax error with
    // following context.
#if 0
    size_t error_context_lines = 5;
    while (true) {
	Gyoji::frontend::yacc::YaccParser::semantic_type lvalue;
	int rc = yylex (&lvalue, scanner);
	if (rc == 0) {
	    break;
	}
	const SourceReference & cur_ref = lex_context->compiler_context.get_token_stream().get_current_source_ref();
	if (cur_ref.get_line() - src_ref.get_line() >= error_context_lines) {
	    break;
	}
    }
#endif    
    auto error = std::make_unique<Gyoji::context::Error>("Syntax Error");
    error->add_message(src_ref, msg);
    return_data.compiler_context.get_errors().add_error(std::move(error));
}
