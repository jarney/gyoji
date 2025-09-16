%{
#include <iostream>
#include <string>
#include <cmath>
#include <memory>
#include <jlang.l.hpp>
#define _JLANG_INTERNAL_COMPILING
#include <jlang-frontend/jsyntax.hpp>
  
 using namespace JLang::frontend::namespaces;
  
%}
 
%require "3.7.4"
%language "C++"
 
%define api.parser.class {YaccParser}
%define api.namespace {JLang::frontend::yacc}
%define api.value.type variant
%param {yyscan_t scanner}
 
%code provides
{
#define DEBUG_NONTERMINALS 0
#if DEBUG_NONTERMINALS
#define PRINT_NONTERMINALS(s) printf("%s\n", s->get_syntax_node()->get_type().c_str())
#else
#define PRINT_NONTERMINALS(s) (0)
#endif

int visibility_from_modifier(JLang::frontend::tree::AccessModifier::AccessModifierType visibility_ast);

#define YY_DECL                                                         \
   int yylex(JLang::frontend::yacc::YaccParser::semantic_type *yylval, yyscan_t yyscanner)
    YY_DECL;
}

%token INVALID_INPUT
%token <JLang::frontend::tree::Terminal_owned_ptr> YYEOF
%token <JLang::frontend::tree::Terminal_owned_ptr> IDENTIFIER
%token <JLang::frontend::tree::Terminal_owned_ptr> NAMESPACE_NAME
%token <JLang::frontend::tree::Terminal_owned_ptr> TYPE_NAME
%token <JLang::frontend::tree::Terminal_owned_ptr> CLASS
%token <JLang::frontend::tree::Terminal_owned_ptr> ENUM

%token <JLang::frontend::tree::Terminal_owned_ptr> NAMESPACE
%token <JLang::frontend::tree::Terminal_owned_ptr> AS
%token <JLang::frontend::tree::Terminal_owned_ptr> USING
%token <JLang::frontend::tree::Terminal_owned_ptr> TYPEDEF
%token <JLang::frontend::tree::Terminal_owned_ptr> STRUCT
%token <JLang::frontend::tree::Terminal_owned_ptr> UNION
%token <JLang::frontend::tree::Terminal_owned_ptr> SIZEOF
%token <JLang::frontend::tree::Terminal_owned_ptr> TYPEOF
%token <JLang::frontend::tree::Terminal_owned_ptr> CAST
%token <JLang::frontend::tree::Terminal_owned_ptr> UNSAFE
 //%token <JLang::frontend::tree::Terminal_owned_ptr> VAR
%token <JLang::frontend::tree::Terminal_owned_ptr> CONST
%token <JLang::frontend::tree::Terminal_owned_ptr> VOLATILE

%token <JLang::frontend::tree::Terminal_owned_ptr> PUBLIC
%token <JLang::frontend::tree::Terminal_owned_ptr> PRIVATE
%token <JLang::frontend::tree::Terminal_owned_ptr> PROTECTED

 /* Logical Operations */
%token <JLang::frontend::tree::Terminal_owned_ptr> LOGICAL_NOT
%token <JLang::frontend::tree::Terminal_owned_ptr> LOGICAL_AND
%token <JLang::frontend::tree::Terminal_owned_ptr>  LOGICAL_OR

 /* Control Flow */
%token <JLang::frontend::tree::Terminal_owned_ptr> RETURN
%token <JLang::frontend::tree::Terminal_owned_ptr> IF
%token <JLang::frontend::tree::Terminal_owned_ptr> ELSE
%token <JLang::frontend::tree::Terminal_owned_ptr> WHILE
%token <JLang::frontend::tree::Terminal_owned_ptr> FOR
%token <JLang::frontend::tree::Terminal_owned_ptr> CONTINUE
%token <JLang::frontend::tree::Terminal_owned_ptr> GOTO
%token <JLang::frontend::tree::Terminal_owned_ptr> LABEL
%token <JLang::frontend::tree::Terminal_owned_ptr> BREAK
%token <JLang::frontend::tree::Terminal_owned_ptr> SWITCH
%token <JLang::frontend::tree::Terminal_owned_ptr> CASE
%token <JLang::frontend::tree::Terminal_owned_ptr> DEFAULT

 /* Binary operations */
%token <JLang::frontend::tree::Terminal_owned_ptr> PLUS
%token <JLang::frontend::tree::Terminal_owned_ptr> MINUS
%token <JLang::frontend::tree::Terminal_owned_ptr> SLASH
%token <JLang::frontend::tree::Terminal_owned_ptr> PERCENT
%token <JLang::frontend::tree::Terminal_owned_ptr> STAR
%token <JLang::frontend::tree::Terminal_owned_ptr> EQUALS
%token <JLang::frontend::tree::Terminal_owned_ptr> BANG
%token <JLang::frontend::tree::Terminal_owned_ptr> TILDE
%token <JLang::frontend::tree::Terminal_owned_ptr> ANDPERSAND
%token <JLang::frontend::tree::Terminal_owned_ptr> MUL_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> DIV_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> MOD_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> ADD_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> SUB_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> LEFT_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> RIGHT_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> AND_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> XOR_ASSIGN
%token <JLang::frontend::tree::Terminal_owned_ptr> OR_ASSIGN

%token <JLang::frontend::tree::Terminal_owned_ptr>  LITERAL_CHAR
%token <JLang::frontend::tree::Terminal_owned_ptr>  LITERAL_FLOAT
%token <JLang::frontend::tree::Terminal_owned_ptr>  LITERAL_INT
%token <JLang::frontend::tree::Terminal_owned_ptr>  LITERAL_STRING

%token NST_COMMENT_MULTILINE
%token NST_COMMENT_SINGLE_LINE
%token NST_WHITESPACE
%token NST_FILE_METADATA

%token <JLang::frontend::tree::Terminal_owned_ptr> LT_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> GT_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> LE_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> GE_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> NE_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> EQ_OP

%token <JLang::frontend::tree::Terminal_owned_ptr> XOR_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> AND_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> PIPE
%token <JLang::frontend::tree::Terminal_owned_ptr> OR_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> LEFT_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> RIGHT_OP

/* Common punctuation */
%token <JLang::frontend::tree::Terminal_owned_ptr> COMMA
%token <JLang::frontend::tree::Terminal_owned_ptr> INC_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> DEC_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> PTR_OP
%token <JLang::frontend::tree::Terminal_owned_ptr> DOT
%token <JLang::frontend::tree::Terminal_owned_ptr> PAREN_L
%token <JLang::frontend::tree::Terminal_owned_ptr> PAREN_R
%token <JLang::frontend::tree::Terminal_owned_ptr> BRACE_L
%token <JLang::frontend::tree::Terminal_owned_ptr> BRACKET_R
%token <JLang::frontend::tree::Terminal_owned_ptr> BRACKET_L
%token <JLang::frontend::tree::Terminal_owned_ptr> BRACE_R
%token <JLang::frontend::tree::Terminal_owned_ptr> SEMICOLON
%token <JLang::frontend::tree::Terminal_owned_ptr> COLON
%token <JLang::frontend::tree::Terminal_owned_ptr> QUESTIONMARK

%token <JLang::frontend::tree::Terminal_owned_ptr> PREC_FIRST
%token <JLang::frontend::tree::Terminal_owned_ptr> PREC_SECOND

%nterm <JLang::frontend::tree::TranslationUnit_owned_ptr> translation_unit;

%nterm <JLang::frontend::tree::FileStatementList_owned_ptr> opt_file_statement_list;
%nterm <JLang::frontend::tree::FileStatementList_owned_ptr> file_statement_list;
%nterm <JLang::frontend::tree::FileStatement_owned_ptr> file_statement;
%nterm <JLang::frontend::tree::FileStatementFunctionDefinition_owned_ptr> file_statement_function_definition;
%nterm <JLang::frontend::tree::FileStatementFunctionDeclaration_owned_ptr> file_statement_function_declaration;
%nterm <JLang::frontend::tree::TypeDefinition_owned_ptr> type_definition;
%nterm <JLang::frontend::tree::ClassDefinition_owned_ptr> class_definition;
%nterm <JLang::frontend::tree::ClassDeclStart_owned_ptr> class_decl_start;
%nterm <JLang::frontend::tree::ClassArgumentList_owned_ptr> opt_class_argument_list;
%nterm <JLang::frontend::tree::ClassArgumentList_owned_ptr> class_argument_list;


%nterm <JLang::frontend::tree::EnumDefinition_owned_ptr> enum_definition;
%nterm <JLang::frontend::tree::EnumDefinitionValueList_owned_ptr> opt_enum_value_list;
%nterm <JLang::frontend::tree::EnumDefinitionValueList_owned_ptr> enum_value_list;
%nterm <JLang::frontend::tree::EnumDefinitionValue_owned_ptr> enum_value;

%nterm <JLang::frontend::tree::UnsafeModifier_owned_ptr> opt_unsafe
%nterm <JLang::frontend::tree::NamespaceDeclaration_owned_ptr> namespace_declaration;
%nterm <JLang::frontend::tree::FileStatementNamespace_owned_ptr> file_statement_namespace;
%nterm <JLang::frontend::tree::UsingAs_owned_ptr> opt_as;
%nterm <JLang::frontend::tree::FileStatementUsing_owned_ptr> file_statement_using;
%nterm <JLang::frontend::tree::FileStatementGlobalDefinition_owned_ptr> file_statement_global_definition;
%nterm <JLang::frontend::tree::GlobalInitializer_owned_ptr> opt_global_initializer;

%nterm <JLang::frontend::tree::GlobalInitializerExpressionPrimary_owned_ptr> global_initializer_expression_primary;
%nterm <JLang::frontend::tree::GlobalInitializerAddressofExpressionPrimary_owned_ptr> global_initializer_addressof_expression_primary;
%nterm <JLang::frontend::tree::GlobalInitializerStructInitializerList_owned_ptr> global_initializer_struct_initializer_list;
%nterm <JLang::frontend::tree::GlobalInitializer_owned_ptr> global_initializer;
%nterm <JLang::frontend::tree::StructInitializerList_owned_ptr> opt_struct_initializer_list;
%nterm <JLang::frontend::tree::StructInitializerList_owned_ptr> struct_initializer_list;
%nterm <JLang::frontend::tree::StructInitializer_owned_ptr> struct_initializer;

%nterm <JLang::frontend::tree::AccessModifier_owned_ptr> opt_access_modifier;
%nterm <JLang::frontend::tree::Terminal_owned_ptr> access_modifier;

%nterm <JLang::frontend::tree::ScopeBody_owned_ptr> scope_body;
%nterm <JLang::frontend::tree::StatementList_owned_ptr> statement_list;
%nterm <JLang::frontend::tree::Statement_owned_ptr> statement;
%nterm <JLang::frontend::tree::StatementVariableDeclaration_owned_ptr> statement_variable_declaration;
%nterm <JLang::frontend::tree::StatementBlock_owned_ptr> statement_block;
%nterm <JLang::frontend::tree::StatementExpression_owned_ptr> statement_expression;
%nterm <JLang::frontend::tree::StatementIfElse_owned_ptr> statement_ifelse;
%nterm <JLang::frontend::tree::StatementWhile_owned_ptr> statement_while;
%nterm <JLang::frontend::tree::StatementFor_owned_ptr> statement_for;
%nterm <JLang::frontend::tree::StatementSwitch_owned_ptr> statement_switch;
%nterm <JLang::frontend::tree::StatementLabel_owned_ptr> statement_label;
%nterm <JLang::frontend::tree::StatementGoto_owned_ptr> statement_goto;
%nterm <JLang::frontend::tree::StatementBreak_owned_ptr> statement_break;
%nterm <JLang::frontend::tree::StatementContinue_owned_ptr> statement_continue;
%nterm <JLang::frontend::tree::StatementReturn_owned_ptr> statement_return;

%nterm <JLang::frontend::tree::StatementSwitchBlock_owned_ptr> statement_switch_block;
%nterm <JLang::frontend::tree::StatementSwitchContent_owned_ptr> statement_switch_content;
%nterm <JLang::frontend::tree::StatementSwitchContent_owned_ptr> opt_statement_switch_content;

%nterm <JLang::frontend::tree::FunctionDefinitionArgList_owned_ptr> opt_function_definition_arg_list;
%nterm <JLang::frontend::tree::FunctionDefinitionArgList_owned_ptr> function_definition_arg_list;
%nterm <JLang::frontend::tree::FunctionDefinitionArg_owned_ptr> function_definition_arg

%nterm <JLang::frontend::tree::ExpressionPrimary_owned_ptr> expression_primary;
%nterm <JLang::frontend::tree::ExpressionPrimaryNested_owned_ptr> expression_primary_nested;
%nterm <JLang::frontend::tree::ExpressionPrimaryIdentifier_owned_ptr> expression_primary_identifier;
%nterm <JLang::frontend::tree::ExpressionPrimaryLiteralInt_owned_ptr> expression_primary_literal_int;
%nterm <JLang::frontend::tree::ExpressionPrimaryLiteralChar_owned_ptr> expression_primary_literal_char;
%nterm <JLang::frontend::tree::ExpressionPrimaryLiteralString_owned_ptr> expression_primary_literal_string;
%nterm <JLang::frontend::tree::ExpressionPrimaryLiteralFloat_owned_ptr> expression_primary_literal_float;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_postfix;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_postfix_primary;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_postfix_arrayindex;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_postfix_function_call;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_postfix_dot;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_postfix_arrow;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_postfix_increment;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_postfix_decrement;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_unary;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_unary_increment;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_unary_decrement;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_unary_prefix;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_unary_sizeof_type;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_cast;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_cast_unary;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_cast_cast;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_multiplicative;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_multiplicative_cast;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_multiplicative_multiply;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_multiplicative_divide;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_multiplicative_modulo;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_additive;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_additive_multiplicative;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_additive_plus;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_additive_minus;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_shift;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_shift_additive;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_shift_left;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_shift_right;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_relational;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_relational_shift;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_relational_gt;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_relational_lt;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_relational_le;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_relational_ge;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_equality;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_and;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_exclusive_or;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_inclusive_or;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_logical_and;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_logical_or;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_conditional;
%nterm <JLang::frontend::tree::Expression_owned_ptr> expression_assignment;

%nterm <JLang::frontend::tree::TypeSpecifier_owned_ptr> type_specifier;
%nterm <JLang::frontend::tree::TypeSpecifierCallArgs_owned_ptr> type_specifier_call_args;
%nterm <JLang::frontend::tree::TypeName_owned_ptr> type_name;
%nterm <JLang::frontend::tree::AccessQualifier_owned_ptr> type_access_qualifier;
%nterm <JLang::frontend::tree::ArrayLength_owned_ptr> opt_array_length;

%nterm <JLang::frontend::tree::ClassMemberDeclarationList_owned_ptr> opt_class_member_declaration_list;
%nterm <JLang::frontend::tree::ClassMemberDeclarationList_owned_ptr> class_member_declaration_list;
%nterm <JLang::frontend::tree::ClassMemberDeclaration_owned_ptr> class_member_declaration;

%nterm <JLang::frontend::tree::Terminal_owned_ptr> operator_unary;
%nterm <JLang::frontend::tree::Terminal_owned_ptr> operator_assignment;

%nterm <JLang::frontend::tree::ArgumentExpressionList_owned_ptr> opt_argument_expression_list;
%nterm <JLang::frontend::tree::ArgumentExpressionList_owned_ptr> argument_expression_list;

%nterm <JLang::frontend::tree::Expression_owned_ptr> expression;

%parse-param {JLang::frontend::yacc::YaccContext & return_data}

%code
{

  extern int lineno;
} // %code

%%

/*** Rules Section ***/
translation_unit
        : opt_file_statement_list YYEOF {
          $$ = std::make_unique<JLang::frontend::tree::TranslationUnit>(std::move($1), std::move($2));
          PRINT_NONTERMINALS($$);
          return_data.set_translation_unit(std::move($$));
        }
        ;

opt_file_statement_list 
        : /**/ YYEOF {
          $$ = std::make_unique<JLang::frontend::tree::FileStatementList>(std::move($1));
          PRINT_NONTERMINALS($$);
        }
        | file_statement_list {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        ;

file_statement_list 
        : file_statement {
          $$ = std::make_unique<JLang::frontend::tree::FileStatementList>();
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
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | file_statement_function_declaration {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | file_statement_global_definition {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | class_definition {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | enum_definition {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | type_definition {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | file_statement_namespace {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        | file_statement_using {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::FileStatement>(
                                                                               std::move($1),
                                                                               sn
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_global_definition
        : opt_access_modifier opt_unsafe type_specifier IDENTIFIER opt_array_length opt_global_initializer SEMICOLON {
          $$ = std::make_unique<JLang::frontend::tree::FileStatementGlobalDefinition>(
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

opt_global_initializer
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::GlobalInitializer>();
                PRINT_NONTERMINALS($$);
        }
        | global_initializer {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

global_initializer
        : global_initializer_expression_primary {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::GlobalInitializer>(std::move($1), sn);
        }
        | global_initializer_addressof_expression_primary {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::GlobalInitializer>(std::move($1), sn);
        }
        | global_initializer_struct_initializer_list {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::GlobalInitializer>(std::move($1), sn);
        }
        ;

global_initializer_expression_primary
        : EQUALS expression_primary {
          $$ = std::make_unique<JLang::frontend::tree::GlobalInitializerExpressionPrimary>(std::move($1),
                                                                                              std::move($2));
          PRINT_NONTERMINALS($$);
        }
        ;

global_initializer_addressof_expression_primary
        : EQUALS ANDPERSAND expression_primary {
          $$ = std::make_unique<JLang::frontend::tree::GlobalInitializerAddressofExpressionPrimary>(std::move($1),
                                                                                                       std::move($2),
                                                                                                       std::move($3)
                                                                                                       );
          PRINT_NONTERMINALS($$);
        }
        ;

global_initializer_struct_initializer_list
        : EQUALS BRACE_L opt_struct_initializer_list BRACE_R {
          $$ = std::make_unique<JLang::frontend::tree::GlobalInitializerStructInitializerList>(std::move($1),
                                                                                                  std::move($2),
                                                                                                  std::move($3),
                                                                                                  std::move($4)
                                                                                                  );
          PRINT_NONTERMINALS($$);
        }
        ;


opt_struct_initializer_list
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::StructInitializerList>();
                PRINT_NONTERMINALS($$);
        }
        | struct_initializer_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

struct_initializer_list
        : struct_initializer {
                $$ = std::make_unique<JLang::frontend::tree::StructInitializerList>();
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
                $$ = std::make_unique<JLang::frontend::tree::StructInitializer>(
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
                $$ = std::make_unique<JLang::frontend::tree::AccessModifier>(
                                                                             JLang::frontend::tree::AccessModifier::AccessModifierType::PUBLIC);
                PRINT_NONTERMINALS($$);
        }
        | access_modifier {
                $$ = std::make_unique<JLang::frontend::tree::AccessModifier>(
                                                                             std::move($1),
                                                                             JLang::frontend::tree::AccessModifier::AccessModifierType::PUBLIC
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
                JLang::frontend::tree::AccessModifier::AccessModifierType access_modifier = $1->get_type();
                std::string namespace_name = $3->value;
                $$ = std::make_unique<JLang::frontend::tree::NamespaceDeclaration>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                return_data.namespace_context.namespace_new(namespace_name, Namespace::TYPE_NAMESPACE, visibility_from_modifier(access_modifier));
                return_data.namespace_context.namespace_push(namespace_name);
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_namespace
        : namespace_declaration BRACE_L opt_file_statement_list BRACE_R SEMICOLON {
          const auto p = $3.get();
                $$ = std::make_unique<JLang::frontend::tree::FileStatementNamespace>(
                                                                                        std::move($1),
                                                                                        std::move($2),
                                                                                        std::move($3),
                                                                                        std::move($4),
                                                                                        std::move($5)
                                                                                        );
                return_data.namespace_context.namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

opt_as
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::UsingAs>();
                PRINT_NONTERMINALS($$);
        }
        | AS IDENTIFIER {
                $$ = std::make_unique<JLang::frontend::tree::UsingAs>(
                                                                std::move($1),
                                                                std::move($2)
                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_using
        : opt_access_modifier USING NAMESPACE NAMESPACE_NAME opt_as SEMICOLON {
                std::string namespace_name = $4->value;
                std::string as_name = $5->get_using_name();
                $$ = std::make_unique<JLang::frontend::tree::FileStatementUsing>(
                                                                                    std::move($1),
                                                                                    std::move($2),
                                                                                    std::move($3),
                                                                                    std::move($4),
                                                                                    std::move($5),
                                                                                    std::move($6)
                                                                                    );
                NamespaceFoundReason::ptr found_std = return_data.namespace_context.namespace_lookup(namespace_name);
                if (!found_std) {
                  fprintf(stderr, "Error: no such namespace %s in using statement\n", namespace_name.c_str());
                  exit(1);
                }
                if (as_name.size() > 0) {
                  return_data.namespace_context.namespace_using(as_name, found_std->location);
                }
                else {
                  return_data.namespace_context.namespace_using("", found_std->location);
                }
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier USING NAMESPACE TYPE_NAME opt_as SEMICOLON {
                std::string namespace_name = $4->value;
                std::string as_name = $5->get_using_name();
                $$ = std::make_unique<JLang::frontend::tree::FileStatementUsing>(
                                                                                    std::move($1),
                                                                                    std::move($2),
                                                                                    std::move($3),
                                                                                    std::move($4),
                                                                                    std::move($5),
                                                                                    std::move($6)
                                                                                    );
                NamespaceFoundReason::ptr found_std = return_data.namespace_context.namespace_lookup(namespace_name);
                if (!found_std) {
                  fprintf(stderr, "Error: no such namespace %s in using statement\n", namespace_name.c_str());
                  exit(1);
                }
                if (as_name.size() > 0) {
                  return_data.namespace_context.namespace_using(as_name, found_std->location);
                }
                else {
                  return_data.namespace_context.namespace_using("", found_std->location);
                }
                PRINT_NONTERMINALS($$);
        }
        ;

class_decl_start
        : opt_access_modifier CLASS IDENTIFIER opt_class_argument_list {
                std::string class_name = $3->value;
                JLang::frontend::tree::AccessModifier::AccessModifierType visibility_modifier = $1->get_type();
                $$ = std::make_unique<JLang::frontend::tree::ClassDeclStart>(
                                                                         std::move($1),
                                                                         std::move($2),
                                                                         std::move($3),
                                                                         std::move($4)
                                                                         );
                return_data.namespace_context.namespace_new(class_name, Namespace::TYPE_CLASS, visibility_from_modifier(visibility_modifier));
                return_data.namespace_context.namespace_push(class_name);
#if 0
                // XXX TODO: This isn't handled correctly for the strongly-typed AST.
                if ($4->children.size() > 0) {
                  for (auto child : $4->children) {
                    if (child->type == Parser::symbol_kind_type::S_class_argument_list) {
                      for (auto grandchild : child->children) {
                        if (grandchild->type == Parser::symbol_kind_type::S_IDENTIFIER) {
                          return_data.namespace_context.namespace_new(grandchild->value, Namespace::TYPE_CLASS, Namespace::VISIBILITY_PRIVATE);
                        }
                      }
                    }
                  }
                }
#endif
                PRINT_NONTERMINALS($$);
        }
        ;

opt_class_argument_list
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::ClassArgumentList>();
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
                $$ = std::make_unique<JLang::frontend::tree::ClassArgumentList>(
                                                                                  std::move($1)
                                                                                  );
                PRINT_NONTERMINALS($$);
        }
        | class_argument_list COMMA IDENTIFIER {
                $$ = std::move($1);
                $$->add_argument(std::move($2), std::move($3));
                PRINT_NONTERMINALS($$);
        }
        ;

class_definition
        : class_decl_start BRACE_L opt_class_member_declaration_list BRACE_R SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::ClassDefinition>(
                                                                                 std::move($1),
                                                                                 std::move($2),
                                                                                 std::move($3),
                                                                                 std::move($4),
                                                                                 std::move($5)
                                                                                 );
                return_data.namespace_context.namespace_pop();
                PRINT_NONTERMINALS($$);
        }
        ;

type_definition
        : opt_access_modifier TYPEDEF type_specifier IDENTIFIER SEMICOLON {
                JLang::frontend::tree::AccessModifier::AccessModifierType visibility_modifier = $1->get_type();
                std::string type_name = $4->value;
                $$ = std::make_unique<JLang::frontend::tree::TypeDefinition>(
                                                                               std::move($1),
                                                                               std::move($2),
                                                                               std::move($3),
                                                                               std::move($4),
                                                                               std::move($5)
                                                                               );
                return_data.namespace_context.namespace_new(type_name, Namespace::TYPE_TYPEDEF, visibility_from_modifier(visibility_modifier));
                PRINT_NONTERMINALS($$);
        }
        ;

enum_definition
        : opt_access_modifier ENUM TYPE_NAME IDENTIFIER BRACE_L opt_enum_value_list BRACE_R SEMICOLON {
                JLang::frontend::tree::AccessModifier::AccessModifierType visibility_modifier = $1->get_type();
                std::string type_name = $4->value;
                $$ = std::make_unique<JLang::frontend::tree::EnumDefinition>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3),
                                                                                std::move($4),
                                                                                std::move($5),
                                                                                std::move($6),
                                                                                std::move($7),
                                                                                std::move($8)
                                                                                );
                return_data.namespace_context.namespace_new(type_name, Namespace::TYPE_TYPEDEF, visibility_from_modifier(visibility_modifier));
                PRINT_NONTERMINALS($$);
        }
        ;

opt_enum_value_list
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::EnumDefinitionValueList>();
                PRINT_NONTERMINALS($$);
        }
        | enum_value_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

enum_value_list
        : enum_value {
                $$ = std::make_unique<JLang::frontend::tree::EnumDefinitionValueList>();
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
        : IDENTIFIER EQUALS expression_primary SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::EnumDefinitionValue>(
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
                $$ = std::make_unique<JLang::frontend::tree::UnsafeModifier>();
                PRINT_NONTERMINALS($$);
        }
        | UNSAFE {
                $$ = std::make_unique<JLang::frontend::tree::UnsafeModifier>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

file_statement_function_declaration
        : opt_access_modifier opt_unsafe type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::FileStatementFunctionDeclaration>(
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

file_statement_function_definition
        : opt_access_modifier opt_unsafe type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R scope_body {
                $$ = std::make_unique<JLang::frontend::tree::FileStatementFunctionDefinition>(
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

opt_function_definition_arg_list
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::FunctionDefinitionArgList>();
                PRINT_NONTERMINALS($$);
        }
        | function_definition_arg_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

function_definition_arg_list
        : function_definition_arg {
                $$ = std::make_unique<JLang::frontend::tree::FunctionDefinitionArgList>();
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
                $$ = std::make_unique<JLang::frontend::tree::FunctionDefinitionArg>(
                                                                                       std::move($1),
                                                                                       std::move($2)
                                                                                       );
                PRINT_NONTERMINALS($$);
        }
        ;

scope_body
        : BRACE_L statement_list BRACE_R {
                $$ = std::make_unique<JLang::frontend::tree::ScopeBody>(
                                                                           std::move($1),
                                                                           std::move($2),
                                                                           std::move($3)
                                                                           );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_list
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::StatementList>();
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
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_block {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_expression {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_ifelse {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_while {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_for {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_switch {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_label {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_goto {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_break {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_continue {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | statement_return {
                $$ = std::make_unique<JLang::frontend::tree::Statement>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

opt_array_length
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::ArrayLength>();
                PRINT_NONTERMINALS($$);
        }
        | BRACKET_L LITERAL_INT BRACKET_R {
                $$ = std::make_unique<JLang::frontend::tree::ArrayLength>(
                                                                             std::move($1),
                                                                             std::move($2),
                                                                             std::move($3)
                                                                             );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_variable_declaration
        : type_specifier IDENTIFIER opt_array_length opt_global_initializer SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::StatementVariableDeclaration>(
                                                                                              std::move($1),
                                                                                              std::move($2),
                                                                                              std::move($3),
                                                                                              std::move($4),
                                                                                              std::move($5)
                                                                                              );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_block
        : opt_unsafe scope_body {
                $$ = std::make_unique<JLang::frontend::tree::StatementBlock>(
                                                                                std::move($1),
                                                                                std::move($2)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_expression
        : expression SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::StatementExpression>(
                                                                                std::move($1),
                                                                                std::move($2)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;
statement_goto
        : GOTO IDENTIFIER SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::StatementGoto>(
                                                                               std::move($1),
                                                                               std::move($2),
                                                                               std::move($3)
                                                                               );
                PRINT_NONTERMINALS($$);
        }
        ;
statement_break
        : BREAK SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::StatementBreak>(
                                                                                std::move($1),
                                                                                std::move($2)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;
statement_continue
        : CONTINUE SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::StatementContinue>(
                                                                                std::move($1),
                                                                                std::move($2)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;
statement_label
        : LABEL IDENTIFIER COLON {
                $$ = std::make_unique<JLang::frontend::tree::StatementLabel>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3)
                                                                                );
                PRINT_NONTERMINALS($$);
        }
        ;

statement_return
        : RETURN expression SEMICOLON {
                $$ = std::make_unique<JLang::frontend::tree::StatementReturn>(
                                                                                 std::move($1),
                                                                                 std::move($2),
                                                                                 std::move($3)
                                                                                 );
                PRINT_NONTERMINALS($$);
        }
        ; 

statement_ifelse
        : IF PAREN_L expression PAREN_R scope_body {
                $$ = std::make_unique<JLang::frontend::tree::StatementIfElse>(
                                                                                 std::move($1),
                                                                                 std::move($2),
                                                                                 std::move($3),
                                                                                 std::move($4),
                                                                                 std::move($5)
                                                                                 );
                PRINT_NONTERMINALS($$);
        }
        | IF PAREN_L expression PAREN_R scope_body ELSE statement_ifelse {
                $$ = std::make_unique<JLang::frontend::tree::StatementIfElse>(
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
                $$ = std::make_unique<JLang::frontend::tree::StatementIfElse>(
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
                $$ = std::make_unique<JLang::frontend::tree::StatementWhile>(
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
                $$ = std::make_unique<JLang::frontend::tree::StatementFor>(
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
        ;

statement_switch
        : SWITCH PAREN_L expression PAREN_R BRACE_L opt_statement_switch_content BRACE_R {
                $$ = std::make_unique<JLang::frontend::tree::StatementSwitch>(
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
                $$ = std::make_unique<JLang::frontend::tree::StatementSwitchContent>();
                PRINT_NONTERMINALS($$);
        }
        | statement_switch_content {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

statement_switch_content
        : statement_switch_block {
                $$ = std::make_unique<JLang::frontend::tree::StatementSwitchContent>();
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
                $$ = std::make_unique<JLang::frontend::tree::StatementSwitchBlock>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                PRINT_NONTERMINALS($$);
        }
        | CASE expression COLON scope_body {
                $$ = std::make_unique<JLang::frontend::tree::StatementSwitchBlock>(
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
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimary>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_nested {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimary>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_int {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimary>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_char {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimary>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_string {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimary>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        | expression_primary_literal_float {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimary>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_identifier
        : IDENTIFIER {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimaryIdentifier>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_literal_int
        : LITERAL_INT {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimaryLiteralInt>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_char
        : LITERAL_CHAR {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimaryLiteralChar>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_string
        : LITERAL_STRING {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimaryLiteralString>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_primary_literal_float
        : LITERAL_FLOAT {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimaryLiteralFloat>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_primary_nested
        : PAREN_L expression PAREN_R {
                $$ = std::make_unique<JLang::frontend::tree::ExpressionPrimaryNested>(
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
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move($1));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_arrayindex
        : expression_postfix BRACKET_L expression BRACKET_R {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionPostfixArrayIndex>(
                                                                                             std::move($1),
                                                                                             std::move($2),
                                                                                             std::move($3),
                                                                                             std::move($4)
                                                                                             );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_function_call
        : expression_postfix PAREN_L opt_argument_expression_list PAREN_R {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionPostfixFunctionCall>(
                                                                                               std::move($1),
                                                                                               std::move($2),
                                                                                               std::move($3),
                                                                                               std::move($4)
                                                                                               );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_dot
        : expression_postfix DOT IDENTIFIER {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionPostfixDot>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_arrow
        : expression_postfix PTR_OP IDENTIFIER {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionPostfixArrow>(
                                                                                        std::move($1),
                                                                                        std::move($2),
                                                                                        std::move($3)
                                                                                        );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_increment
        : expression_postfix INC_OP {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionPostfixIncDec>(
                                                                                         std::move($1),
                                                                                         std::move($2)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_postfix_decrement
        : expression_postfix DEC_OP {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionPostfixIncDec>(
                                                                                         std::move($1),
                                                                                         std::move($2)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
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
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionUnaryPrefix>(
                                                                                       std::move($1),
                                                                                       std::move($2)
                                                                                       );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_decrement
        : DEC_OP expression_unary {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionUnaryPrefix>(
                                                                                       std::move($1),
                                                                                       std::move($2)
                                                                                       );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_prefix
        : operator_unary expression_cast {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionUnaryPrefix>(
                                                                                       std::move($1),
                                                                                       std::move($2)
                                                                                       );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_unary_sizeof_type
        : SIZEOF PAREN_L type_specifier PAREN_R {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionUnarySizeofType>(
                                                                                       std::move($1),
                                                                                       std::move($2),
                                                                                       std::move($3),
                                                                                       std::move($4)
                                                                                       );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;



operator_unary
        : ANDPERSAND {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | STAR {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | PLUS {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | MINUS {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | TILDE {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | BANG {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
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
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionCast>(
                                                                                std::move($1),
                                                                                std::move($2),
                                                                                std::move($3),
                                                                                std::move($4),
                                                                                std::move($5),
                                                                                std::move($6)
                                                                                );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
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
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_multiplicative_divide
        : expression_multiplicative SLASH expression_cast {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_multiplicative_modulo
        : expression_multiplicative PERCENT expression_cast {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
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
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_additive_minus
        : expression_additive MINUS expression_multiplicative {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
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
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_shift_right
        : expression_shift RIGHT_OP expression_additive {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
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
        : expression_relational LT_OP expression_shift {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_gt
        : expression_relational GT_OP expression_shift {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_le
        : expression_relational LE_OP expression_shift {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;
expression_relational_ge
        : expression_relational GE_OP expression_shift {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        ;

expression_equality
        : expression_relational {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        | expression_equality EQ_OP expression_relational {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
        | expression_equality NE_OP expression_relational {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
	;

expression_and
        : expression_equality {
          $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_and ANDPERSAND expression_equality {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
	;

expression_exclusive_or
        : expression_and {
          $$ = std::move($1);
          PRINT_NONTERMINALS($$);
        }
        | expression_exclusive_or XOR_OP expression_and {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
	;

expression_inclusive_or
        : expression_exclusive_or {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_inclusive_or PIPE expression_exclusive_or {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
	;

expression_logical_and
        : expression_inclusive_or {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_and AND_OP expression_inclusive_or {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
	;

expression_logical_or
        : expression_logical_and {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_or OR_OP expression_logical_and {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
	;

expression_conditional
        : expression_logical_or {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_logical_or QUESTIONMARK expression COLON expression_conditional {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionTrinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3),
                                                                                         std::move($4),
                                                                                         std::move($5)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
	;

expression_assignment
        : expression_conditional {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        | expression_unary operator_assignment expression_assignment {
                auto expr = std::make_unique<JLang::frontend::tree::ExpressionBinary>(
                                                                                         std::move($1),
                                                                                         std::move($2),
                                                                                         std::move($3)
                                                                                         );
                $$ = std::make_unique<JLang::frontend::tree::Expression>(std::move(expr));
                PRINT_NONTERMINALS($$);
        }
	;

operator_assignment
        : EQUALS {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| MUL_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| DIV_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| MOD_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| ADD_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| SUB_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| LEFT_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| RIGHT_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| AND_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        } 
	| XOR_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
	| OR_ASSIGN {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
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
                $$ = std::make_unique<JLang::frontend::tree::TypeName>(
                                                                          std::move($1),
                                                                          std::move($2),
                                                                          std::move($3),
                                                                          std::move($4)
                                                                          );
                PRINT_NONTERMINALS($$);
        }
        | TYPE_NAME {
                $$ = std::make_unique<JLang::frontend::tree::TypeName>(
                                                                          std::move($1)
                                                                          );
                PRINT_NONTERMINALS($$);
        }
        ;

opt_class_member_declaration_list
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclarationList>();
                PRINT_NONTERMINALS($$);
        }
        | class_member_declaration_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

class_member_declaration_list
        : class_member_declaration {
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclarationList>();
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
        : opt_access_modifier type_specifier IDENTIFIER opt_array_length SEMICOLON {
                // Member Variable
                auto expr = std::make_unique<JLang::frontend::tree::ClassMemberDeclarationVariable>(
                                                                                                       std::move($1),
                                                                                                       std::move($2),
                                                                                                       std::move($3),
                                                                                                       std::move($4),
                                                                                                       std::move($5)
                                                                                                       );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclaration>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier type_specifier IDENTIFIER PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                // Method
                auto expr = std::make_unique<JLang::frontend::tree::ClassMemberDeclarationMethod>(
                                                                                                     std::move($1),
                                                                                                     std::move($2),
                                                                                                     std::move($3),
                                                                                                     std::move($4),
                                                                                                     std::move($5),
                                                                                                     std::move($6),
                                                                                                     std::move($7)
                                                                                                     );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclaration>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier type_specifier PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                // Constructor
                auto expr = std::make_unique<JLang::frontend::tree::ClassMemberDeclarationConstructor>(
                                                                                                     std::move($1),
                                                                                                     std::move($2),
                                                                                                     std::move($3),
                                                                                                     std::move($4),
                                                                                                     std::move($5),
                                                                                                     std::move($6)
                                                                                                     );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclaration>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | opt_access_modifier TILDE type_specifier PAREN_L opt_function_definition_arg_list PAREN_R SEMICOLON {
                // Destructor
                auto expr = std::make_unique<JLang::frontend::tree::ClassMemberDeclarationDestructor>(
                                                                                                         std::move($1),
                                                                                                         std::move($2),
                                                                                                         std::move($3),
                                                                                                         std::move($4),
                                                                                                         std::move($5),
                                                                                                         std::move($6),
                                                                                                         std::move($7)
                                                                                                         );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclaration>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | class_definition {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclaration>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | enum_definition {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclaration>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_definition {
                const JLang::frontend::ast::SyntaxNode &sn = *($1);
                $$ = std::make_unique<JLang::frontend::tree::ClassMemberDeclaration>(std::move($1), sn);
                PRINT_NONTERMINALS($$);
        }
        ;


type_access_qualifier
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::AccessQualifier>(
                                                                              JLang::frontend::tree::AccessQualifier::AccessQualifierType::UNSPECIFIED
                                                                              );
                PRINT_NONTERMINALS($$);
        }
        | CONST {
                $$ = std::make_unique<JLang::frontend::tree::AccessQualifier>(
                                                                              std::move($1),
                                                                              JLang::frontend::tree::AccessQualifier::AccessQualifierType::CONST
                                                                              );
                PRINT_NONTERMINALS($$);
        }
        | VOLATILE {
                $$ = std::make_unique<JLang::frontend::tree::AccessQualifier>(
                                                                                 std::move($1),
                                                                                 JLang::frontend::tree::AccessQualifier::AccessQualifierType::VOLATILE
                                                                                 );
                PRINT_NONTERMINALS($$);
        }
        ;

type_specifier_call_args
        : type_specifier {
                $$ = std::make_unique<JLang::frontend::tree::TypeSpecifierCallArgs>();
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
                auto expr = std::make_unique<JLang::frontend::tree::TypeSpecifierSimple>(
                                                                                      std::move($1),
                                                                                      std::move($2)
                                                                                      );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier PAREN_L type_specifier_call_args PAREN_R {
                auto expr = std::make_unique<JLang::frontend::tree::TypeSpecifierTemplate>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3),
                                                                                      std::move($4)
                                                                                      );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier PAREN_L STAR IDENTIFIER PAREN_R PAREN_L opt_function_definition_arg_list PAREN_R {
                auto expr = std::make_unique<JLang::frontend::tree::TypeSpecifierFunctionPointer>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3),
                                                                                      std::move($4),
                                                                                      std::move($5),
                                                                                      std::move($6),
                                                                                      std::move($7),
                                                                                      std::move($8)
                                                                                      );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier STAR type_access_qualifier {
                auto expr = std::make_unique<JLang::frontend::tree::TypeSpecifierPointerTo>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        | type_specifier ANDPERSAND type_access_qualifier {
                auto expr = std::make_unique<JLang::frontend::tree::TypeSpecifierReferenceTo>(
                                                                                      std::move($1),
                                                                                      std::move($2),
                                                                                      std::move($3)
                                                                                      );
                const JLang::frontend::ast::SyntaxNode &sn = *(expr);
                $$ = std::make_unique<JLang::frontend::tree::TypeSpecifier>(std::move(expr), sn);
                PRINT_NONTERMINALS($$);
        }
        ;

opt_argument_expression_list
        : /**/ {
                $$ = std::make_unique<JLang::frontend::tree::ArgumentExpressionList>();
                PRINT_NONTERMINALS($$);
        }
        | argument_expression_list {
                $$ = std::move($1);
                PRINT_NONTERMINALS($$);
        }
        ;

argument_expression_list
        : expression {
                $$ = std::make_unique<JLang::frontend::tree::ArgumentExpressionList>();
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

int visibility_from_modifier(JLang::frontend::tree::AccessModifier::AccessModifierType visibility_ast)
{
    switch (visibility_ast) {
    case JLang::frontend::tree::AccessModifier::AccessModifierType::PUBLIC:
      return Namespace::VISIBILITY_PUBLIC;
    case JLang::frontend::tree::AccessModifier::AccessModifierType::PROTECTED:
      return Namespace::VISIBILITY_PROTECTED;
    case JLang::frontend::tree::AccessModifier::AccessModifierType::PRIVATE:
      return Namespace::VISIBILITY_PRIVATE;
    default:
      return Namespace::VISIBILITY_PUBLIC;
    }
}

void JLang::frontend::yacc::YaccParser::error(const std::string& msg) {
    printf("Syntax error at line %d : %s\n", lineno, msg.c_str());
}
