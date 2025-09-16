#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once


/*!
 *  \addtogroup Frontend
 *  @{
 */

//! Parse Tree Data
/*!
 * Strongly-typed representation of the parse tree
 * resulting from reading and parsing an input file.
 */
namespace JLang::frontend::tree {
    /**
     * This class represents data obtained from the
     * lexical stage that did not affect the syntax.
     * This data includes whitespace, comments, and
     * file metadata.  This data is useful to track because
     * it may be used in code-formatters to faithfully
     * reproduce the input and is also useful in providing
     * meaningful error messaging with context surrounding
     * the error.
     */
    class TerminalNonSyntax {
    public:
      typedef enum {
          EXTRA_COMMENT_SINGLE_LINE,
          EXTRA_COMMENT_MULTI_LINE,
          EXTRA_WHITESPACE,
          EXTRA_FILE_METADATA
      }  Type;
    
      TerminalNonSyntax(Type _type, std::string _data);
      ~TerminalNonSyntax();

      /**
       * This method returns the type of data
       * from the enum above.
       */
      const Type &get_type() const;
      /**
       * This method provides access to the raw input
       * for the type of non-syntax data available.
       */
      const std::string & get_data() const;

      void append(std::string _data);
      
    private:
      Type type;
      std::string data;
    };
    typedef std::unique_ptr<TerminalNonSyntax> TerminalNonSyntax_owned_ptr;

    /**
     * Terminals are the raw tokens received by the lexer.
     * They carry the token information as well as any
     * "Non-syntax" data like comments and whitespace.
     */
    class Terminal : public JLang::frontend::ast::SyntaxNode {
    public:
      Terminal();
      ~Terminal();
      int type;
      std::string typestr;
      std::string value;
      size_t lineno;
      size_t colno;
      std::string fully_qualified_name;
      
      // The terminal "owns" uniquely all of the non-syntax data
      // in this vector.  It may be returned to access it,
      // but these are owned pointers, so they must only
      // be de-referenced and never assigned to
      std::vector<TerminalNonSyntax_owned_ptr> non_syntax;
    };


    class FileStatement : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        FileStatementFunctionDefinition_owned_ptr,
        FileStatementFunctionDeclaration_owned_ptr,
        FileStatementGlobalDefinition_owned_ptr,
        ClassDefinition_owned_ptr,
        EnumDefinition_owned_ptr,
        TypeDefinition_owned_ptr,
        FileStatementNamespace_owned_ptr,
        FileStatementUsing_owned_ptr> FileStatementType;

      FileStatement(FileStatementType _statement, const JLang::frontend::ast::SyntaxNode & _sn);
      ~FileStatement();

      const FileStatementType & get_statement() const;

    private:
      FileStatementType statement;
    };

    class AccessQualifier : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef enum {
        UNSPECIFIED,
        VOLATILE,
        CONST
      } AccessQualifierType;
      AccessQualifier(AccessQualifier::AccessQualifierType _type);
      AccessQualifier(Terminal_owned_ptr _qualifier, AccessQualifierType _type);
      ~AccessQualifier();
      const AccessQualifier::AccessQualifierType & get_type() const;
    private:
      AccessQualifier::AccessQualifierType type;
      Terminal_owned_ptr qualifier;
    };
    
    class AccessModifier : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef enum {
        PUBLIC,
        PROTECTED,
        PRIVATE
      } AccessModifierType;
      AccessModifier(Terminal_owned_ptr _modifier, AccessModifierType _type);
      AccessModifier(AccessModifier::AccessModifierType _type);
      ~AccessModifier();
      const AccessModifierType & get_type() const;
    private:
      AccessModifierType type;
      Terminal_owned_ptr modifier;
    };

    class UnsafeModifier : public JLang::frontend::ast::SyntaxNode {
    public:
      UnsafeModifier(Terminal_owned_ptr _unsafe_token);
      UnsafeModifier();
      ~UnsafeModifier();
      bool is_unsafe() const;
    private:
      Terminal_owned_ptr unsafe_token;
      
    };

    class TypeName : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeName(Terminal_owned_ptr _type_name);
      TypeName(Terminal_owned_ptr _typeof_token,
               Terminal_owned_ptr _paren_l_token,
               Expression_owned_ptr _expression,
               Terminal_owned_ptr _paren_r_token
               );
      ~TypeName();
      bool is_expression() const;
      const std::string & get_name() const;
      const Expression & get_expression() const;
    private:
      bool m_is_expression;
      // For raw names
      Terminal_owned_ptr type_name;
      // for typeof expressions
      Terminal_owned_ptr typeof_token;
      Terminal_owned_ptr paren_l_token;
      Expression_owned_ptr expression;
      Terminal_owned_ptr paren_r_token;
    };
    
    class TypeSpecifierCallArgs : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierCallArgs();
      ~TypeSpecifierCallArgs();
      const std::vector<TypeSpecifier_owned_ptr> & get_arguments() const;
      void add_argument(TypeSpecifier_owned_ptr _argument);
      void add_argument(Terminal_owned_ptr _comma_token, TypeSpecifier_owned_ptr _argument);
    private:
      std::vector<Terminal_owned_ptr> comma_list;
      std::vector<TypeSpecifier_owned_ptr> arguments;
    };

    class TypeSpecifierSimple : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierSimple(
                          AccessQualifier_owned_ptr _access_qualifier,
                          TypeName_owned_ptr _type_name
                          );
      ~TypeSpecifierSimple();
      const AccessQualifier & get_access_qualifier() const;
      const TypeName & get_type_name() const;
    private:
      AccessQualifier_owned_ptr access_qualifier;
      TypeName_owned_ptr type_name;
    };
    class TypeSpecifierTemplate : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierTemplate(
                            TypeSpecifier_owned_ptr _type_specifier,
                            Terminal_owned_ptr _paren_l_token,
                            TypeSpecifierCallArgs_owned_ptr _type_specifier_call_args,
                            Terminal_owned_ptr _paren_r_token
                            );
      ~TypeSpecifierTemplate();
      const TypeSpecifier & get_type() const;
      const TypeSpecifierCallArgs & get_args() const;
    private:
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr paren_l_token;
      TypeSpecifierCallArgs_owned_ptr type_specifier_call_args;
      Terminal_owned_ptr paren_r_token;
    };
    class TypeSpecifierFunctionPointer : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierFunctionPointer(
                                   TypeSpecifier_owned_ptr _type_specifier,
                                   Terminal_owned_ptr _paren_l1_token,
                                   Terminal_owned_ptr _star_token,
                                   Terminal_owned_ptr _identifier_token,
                                   Terminal_owned_ptr _paren_r1_token,
                                   Terminal_owned_ptr _paren_l2_token,
                                   FunctionDefinitionArgList_owned_ptr _function_definition_arg_list,
                                   Terminal_owned_ptr _paren_r2_token
                                   );
      ~TypeSpecifierFunctionPointer();
      const TypeSpecifier & get_return_type() const;
      const std::string & get_name() const;
      const FunctionDefinitionArgList & get_args() const;
    private:
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr paren_l1_token;
      Terminal_owned_ptr star_token;
      Terminal_owned_ptr identifier_token;
      Terminal_owned_ptr paren_r1_token;
      Terminal_owned_ptr paren_l2_token;
      FunctionDefinitionArgList_owned_ptr function_definition_arg_list;
      Terminal_owned_ptr paren_r2_token;
    };
    class TypeSpecifierPointerTo : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierPointerTo(
                             TypeSpecifier_owned_ptr _type_specifier,
                             Terminal_owned_ptr _star_token,
                             AccessQualifier_owned_ptr _access_qualifier
                             );
      ~TypeSpecifierPointerTo();
      const TypeSpecifier & get_type_specifier() const;
      const AccessQualifier & get_access_qualifier() const;
    private:
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr star_token;
      AccessQualifier_owned_ptr access_qualifier;
    };
    class TypeSpecifierReferenceTo : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierReferenceTo(
                               TypeSpecifier_owned_ptr _type_specifier,
                               Terminal_owned_ptr _andpersand_token,
                               AccessQualifier_owned_ptr _access_qualifier
                               );
      ~TypeSpecifierReferenceTo();
      const TypeSpecifier & get_type_specifier() const;
      const AccessQualifier & get_access_qualifier() const;
    private:
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr andpersand_token;
      AccessQualifier_owned_ptr access_qualifier;
    };
    
    class TypeSpecifier : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        TypeSpecifierSimple_owned_ptr,
        TypeSpecifierTemplate_owned_ptr,
        TypeSpecifierFunctionPointer_owned_ptr,
        TypeSpecifierPointerTo_owned_ptr,
        TypeSpecifierReferenceTo_owned_ptr
      > TypeSpecifierType;
      TypeSpecifier(TypeSpecifier::TypeSpecifierType _type, const JLang::frontend::ast::SyntaxNode & _sn);
      ~TypeSpecifier();
      const TypeSpecifier::TypeSpecifierType & get_type() const;
    private:
      TypeSpecifier::TypeSpecifierType type;
    };

    class FunctionDefinitionArg : public JLang::frontend::ast::SyntaxNode {
    public:
      FunctionDefinitionArg(TypeSpecifier_owned_ptr _type_specifier,
                            Terminal_owned_ptr _identifier_token
                            );
      ~FunctionDefinitionArg();
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
    private:
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr identifier_token;
    };
    class FunctionDefinitionArgList : public JLang::frontend::ast::SyntaxNode {
    public:
      FunctionDefinitionArgList();
      ~FunctionDefinitionArgList();
      const std::vector<FunctionDefinitionArg_owned_ptr> & get_arguments() const;
      void add_argument(FunctionDefinitionArg_owned_ptr _argument);
      void add_comma(Terminal_owned_ptr _comma);
    private:
      std::vector<Terminal_owned_ptr> commas;
      std::vector<FunctionDefinitionArg_owned_ptr> arguments;
    };

    class FileStatementFunctionDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementFunctionDeclaration(
                                      AccessModifier_owned_ptr _access_modifier,
                                      UnsafeModifier_owned_ptr _unsafe_modifier,
                                      TypeSpecifier_owned_ptr _type_specifier,
                                      Terminal_owned_ptr _name,
                                      Terminal_owned_ptr _paren_l,
                                      FunctionDefinitionArgList_owned_ptr _arguments,
                                      Terminal_owned_ptr _paren_r,
                                      Terminal_owned_ptr _semicolon
                                      );
      ~FileStatementFunctionDeclaration();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
      
    private:
      AccessModifier_owned_ptr access_modifier;
      UnsafeModifier_owned_ptr unsafe_modifier;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr name; // function name (IDENTIFIER)
      Terminal_owned_ptr paren_l; // argument list delimiter PAREN_L
      FunctionDefinitionArgList_owned_ptr arguments;
      Terminal_owned_ptr paren_r; // argument list delimiter PAREN_R
      Terminal_owned_ptr semicolon; // argument list delimiter SEMICOLON
    };


    class StatementVariableDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementVariableDeclaration(
                                   TypeSpecifier_owned_ptr _type_specifier,
                                   Terminal_owned_ptr _identifier_token,
                                   ArrayLength_owned_ptr _array_length,
                                   GlobalInitializer_owned_ptr _global_initializer,
                                   Terminal_owned_ptr _semicolon_token
                                   );
      ~StatementVariableDeclaration();
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const ArrayLength & get_array_length() const;
      const GlobalInitializer & get_initializer() const;
    private:
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr identifier_token;
      ArrayLength_owned_ptr array_length;
      GlobalInitializer_owned_ptr global_initializer;
      Terminal_owned_ptr semicolon_token;
    private:
    };
    class StatementBlock : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementBlock(
                     UnsafeModifier_owned_ptr _unsafe_modifier,
                     ScopeBody_owned_ptr _scope_body
                     );
      ~StatementBlock();
      const UnsafeModifier & get_unsafe_modifier() const;
      const ScopeBody & get_scope_body() const;
    private:
      UnsafeModifier_owned_ptr unsafe_modifier;
      ScopeBody_owned_ptr scope_body;
    private:
    };
    class StatementExpression : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementExpression(
                          Expression_owned_ptr _expression,
                          Terminal_owned_ptr _semicolon_token
                          );
      ~StatementExpression();
      const Expression & get_expression() const;
    private:
      Expression_owned_ptr expression;
      Terminal_owned_ptr semicolon_token;
    };
    class StatementIfElse;
    class StatementIfElse : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementIfElse(
                      Terminal_owned_ptr _if_token,
                      Terminal_owned_ptr _paren_l_token,
                      Expression_owned_ptr _expression,
                      Terminal_owned_ptr _paren_r_token,
                      ScopeBody_owned_ptr _if_scope_body,
                      Terminal_owned_ptr _else_token,
                      ScopeBody_owned_ptr _else_scope_body
                      );
      StatementIfElse(
                      Terminal_owned_ptr _if_token,
                      Terminal_owned_ptr _paren_l_token,
                      Expression_owned_ptr _expression,
                      Terminal_owned_ptr _paren_r_token,
                      ScopeBody_owned_ptr _if_scope_body,
                      Terminal_owned_ptr _else_token,
                      StatementIfElse_owned_ptr _statement_if_else
                      );
      StatementIfElse(
                      Terminal_owned_ptr _if_token,
                      Terminal_owned_ptr _paren_l_token,
                      Expression_owned_ptr _expression,
                      Terminal_owned_ptr _paren_r_token,
                      ScopeBody_owned_ptr _if_scope_body
                      );
      ~StatementIfElse();
      const Expression & get_expression() const;
      const ScopeBody & get_if_scope_body() const;
      const StatementIfElse & get_else_if() const;
      const ScopeBody & get_else_scope_body() const;
      bool has_else() const;
      bool has_else_if() const;
    private:
      bool m_has_else;
      bool m_has_else_if;
      Terminal_owned_ptr if_token;
      Terminal_owned_ptr paren_l_token;
      Expression_owned_ptr expression;
      Terminal_owned_ptr paren_r_token;
      ScopeBody_owned_ptr if_scope_body;
      Terminal_owned_ptr else_token;
      ScopeBody_owned_ptr else_scope_body;
      StatementIfElse_owned_ptr else_if;
    };
    class StatementWhile : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementWhile(
                     Terminal_owned_ptr _while_token,
                     Terminal_owned_ptr _paren_l_token,
                     Expression_owned_ptr _expression,
                     Terminal_owned_ptr _paren_r_token,
                     ScopeBody_owned_ptr _scope_body
                     );
     ~StatementWhile();
      const Expression & get_expression() const;
      const ScopeBody & get_scope_body() const;
    private:
      Terminal_owned_ptr while_token;
      Terminal_owned_ptr paren_l_token;
      Expression_owned_ptr expression;
      Terminal_owned_ptr paren_r_token;
      ScopeBody_owned_ptr scope_body;
    };
    class StatementFor : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementFor(
                   Terminal_owned_ptr _for_token,
                   Terminal_owned_ptr _paren_l_token,
                   Expression_owned_ptr _expression_initial,
                   Terminal_owned_ptr _semicolon_initial,
                   Expression_owned_ptr _expression_termination,
                   Terminal_owned_ptr _semicolon_termination,
                   Expression_owned_ptr _expression_increment,
                   Terminal_owned_ptr _paren_r_token,
                   ScopeBody_owned_ptr _scope_body
                   );
      ~StatementFor();
      const Expression & get_expression_initial() const;
      const Expression & get_expression_termination() const;
      const Expression & get_expression_increment() const;
      const ScopeBody & get_scope_body() const;
    private:
      Terminal_owned_ptr for_token;
      Terminal_owned_ptr paren_l_token;
      Expression_owned_ptr expression_initial;
      Terminal_owned_ptr semicolon_initial;
      Expression_owned_ptr expression_termination;
      Terminal_owned_ptr semicolon_termination;
      Expression_owned_ptr expression_increment;
      Terminal_owned_ptr paren_r_token;
      ScopeBody_owned_ptr scope_body;
    };

    class StatementSwitchBlock : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitchBlock(
                           Terminal_owned_ptr _default_token,
                           Terminal_owned_ptr _colon_token,
                           ScopeBody_owned_ptr _scope_body
                           );
      StatementSwitchBlock(
                           Terminal_owned_ptr _case_token,
                           Expression_owned_ptr _expression,
                           Terminal_owned_ptr _colon_token,
                           ScopeBody_owned_ptr _scope_body
                           );
      ~StatementSwitchBlock();
      bool is_default() const;
      const Expression & get_expression();
      const ScopeBody & get_scope_body();
    private:
      bool m_is_default;
      // For the default case
      Terminal_owned_ptr default_token;
      // For the expression case
      Terminal_owned_ptr case_token;
      Expression_owned_ptr expression;
      // Common:
      Terminal_owned_ptr colon_token;
      ScopeBody_owned_ptr scope_body;
    };
    
    class StatementSwitchContent : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitchContent();
      ~StatementSwitchContent();
      const std::vector<StatementSwitchBlock_owned_ptr> & get_blocks() const;
      void add_block(StatementSwitchBlock_owned_ptr _block);
    private:
      std::vector<StatementSwitchBlock_owned_ptr> blocks;
    };
    
    class StatementSwitch : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitch(
                      Terminal_owned_ptr _switch_token,
                      Terminal_owned_ptr _paren_l_token,
                      Expression_owned_ptr expression,
                      Terminal_owned_ptr _paren_r_token,
                      Terminal_owned_ptr _brace_l_token,
                      StatementSwitchContent_owned_ptr _switch_content,
                      Terminal_owned_ptr _brace_r_token
                      );
      ~StatementSwitch();
      const Expression & get_expression() const;
      const StatementSwitchContent & get_switch_content() const;
    private:
      Terminal_owned_ptr switch_token;
      Terminal_owned_ptr paren_l_token;
      Expression_owned_ptr expression;
      Terminal_owned_ptr paren_r_token;
      Terminal_owned_ptr brace_l_token;
      StatementSwitchContent_owned_ptr switch_content;
      Terminal_owned_ptr brace_r_token;
    };
    class StatementLabel : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementLabel(
                     Terminal_owned_ptr _label_token,
                     Terminal_owned_ptr _identifier_token,
                     Terminal_owned_ptr _colon_token
                     );
      ~StatementLabel();
      const std::string & get_name() const;
    private:
      Terminal_owned_ptr label_token;
      Terminal_owned_ptr identifier_token;
      Terminal_owned_ptr colon_token;
    };
    class StatementGoto : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementGoto(
                    Terminal_owned_ptr _goto_token,
                    Terminal_owned_ptr _identifier_token,
                    Terminal_owned_ptr _semicolon_token
                    );
      ~StatementGoto();
      const std::string & get_label() const;
    private:
      Terminal_owned_ptr goto_token;
      Terminal_owned_ptr identifier_token;
      Terminal_owned_ptr semicolon_token;
    };
    class StatementBreak : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementBreak(
                     Terminal_owned_ptr _break_token,
                     Terminal_owned_ptr _semicolon_token
                     );
      ~StatementBreak();
    private:
      Terminal_owned_ptr break_token;
      Terminal_owned_ptr semicolon_token;
    };
    class StatementContinue : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementContinue(
                        Terminal_owned_ptr _continue_token,
                        Terminal_owned_ptr _semicolon_token
                        );
      ~StatementContinue();
    private:
      Terminal_owned_ptr continue_token;
      Terminal_owned_ptr semicolon_token;
    };
    class StatementReturn : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementReturn(
                      Terminal_owned_ptr _return_token,
                      Expression_owned_ptr _expression,
                      Terminal_owned_ptr _semicolon_token
                      );
      ~StatementReturn();
      const Expression & get_expression() const;
    private:
      Terminal_owned_ptr return_token;
      Expression_owned_ptr expression;
      Terminal_owned_ptr semicolon_token;
    };

    class Statement : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
            StatementVariableDeclaration_owned_ptr,
            StatementBlock_owned_ptr,
            StatementExpression_owned_ptr,
            StatementIfElse_owned_ptr,
            StatementWhile_owned_ptr,
            StatementFor_owned_ptr,
            StatementSwitch_owned_ptr,
            StatementLabel_owned_ptr,
            StatementGoto_owned_ptr,
            StatementBreak_owned_ptr,
            StatementContinue_owned_ptr,
            StatementReturn_owned_ptr
      > StatementType;

      Statement(StatementType _statement, const SyntaxNode & _sn);
      ~Statement();
      const StatementType & get_statement() const;
    private:
      StatementType statement;
    };
    
    class StatementList : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementList();
      ~StatementList();
      void add_statement(Statement_owned_ptr _statement);
      const std::vector<Statement_owned_ptr> &get_statements() const;
    private:
      std::vector<Statement_owned_ptr> statements;
    };
    
    class ScopeBody : public JLang::frontend::ast::SyntaxNode {
    public:
      ScopeBody(
                Terminal_owned_ptr brace_l_token,
                StatementList_owned_ptr statement_list,
                Terminal_owned_ptr brace_r_token
                );
      ~ScopeBody();
      const StatementList & get_statements() const;
    private:
      Terminal_owned_ptr brace_l_token;
      StatementList_owned_ptr statement_list;
      Terminal_owned_ptr brace_r_token;
    };
    
    class FileStatementFunctionDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementFunctionDefinition(
                                      AccessModifier_owned_ptr _access_modifier,
                                      UnsafeModifier_owned_ptr _unsafe_modifier,
                                      TypeSpecifier_owned_ptr _type_specifier,
                                      Terminal_owned_ptr _name,
                                      Terminal_owned_ptr _paren_l,
                                      FunctionDefinitionArgList_owned_ptr _arguments,
                                      Terminal_owned_ptr _paren_r,
                                      ScopeBody_owned_ptr _scope_body
                                      );
      ~FileStatementFunctionDefinition();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
      const ScopeBody & get_scope_body() const;
      
    private:
      AccessModifier_owned_ptr access_modifier;
      UnsafeModifier_owned_ptr unsafe_modifier;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr name; // function name (IDENTIFIER)
      Terminal_owned_ptr paren_l; // argument list delimiter PAREN_L
      FunctionDefinitionArgList_owned_ptr arguments;
      Terminal_owned_ptr paren_r; // argument list delimiter PAREN_R
      ScopeBody_owned_ptr scope_body; // argument list delimiter SEMICOLON
    };

    class ArrayLength : public JLang::frontend::ast::SyntaxNode {
    public:
      ArrayLength();
      ArrayLength(
                  Terminal_owned_ptr _bracket_l_token,
                  Terminal_owned_ptr _literal_int_token,
                  Terminal_owned_ptr _bracket_r_token
                  );
      ~ArrayLength();
      bool is_array() const;
      size_t get_size() const;
    private:
      Terminal_owned_ptr bracket_l_token;
      Terminal_owned_ptr literal_int_token;
      Terminal_owned_ptr bracket_r_token;
    };

    class ClassDeclStart : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassDeclStart(
                     AccessModifier_owned_ptr _access_modifier,
                     Terminal_owned_ptr _class_token,
                     Terminal_owned_ptr _identifier_token,
                     ClassArgumentList_owned_ptr _class_argument_list
                     );
                     
      ~ClassDeclStart();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const ClassArgumentList & get_argument_list() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      Terminal_owned_ptr class_token;
      Terminal_owned_ptr identifier_token;
      ClassArgumentList_owned_ptr class_argument_list;
    };
    class ClassArgumentList : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassArgumentList();
      ClassArgumentList(Terminal_owned_ptr _argument);
      ~ClassArgumentList();
      void add_argument(Terminal_owned_ptr _comma, Terminal_owned_ptr _argument);
      void add_parens(Terminal_owned_ptr _paren_l, Terminal_owned_ptr _paren_r);
      const std::vector<Terminal_owned_ptr> & get_arguments() const;
    private:
      Terminal_owned_ptr paren_l;
      std::vector<Terminal_owned_ptr> comma_list;
      std::vector<Terminal_owned_ptr> argument_list;
      Terminal_owned_ptr paren_r;
    };

    class ClassMemberDeclarationVariable : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationVariable(
                                     AccessModifier_owned_ptr _access_modifier,
                                     TypeSpecifier_owned_ptr _type_specifier,
                                     Terminal_owned_ptr _identifier_token,
                                     ArrayLength_owned_ptr _array_length,
                                     Terminal_owned_ptr _semicolon_token
                                     );
      ~ClassMemberDeclarationVariable();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const ArrayLength & get_array_length() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr identifier_token;
      ArrayLength_owned_ptr array_length;
      Terminal_owned_ptr semicolon_token;
    };    
    class ClassMemberDeclarationMethod : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationMethod(
                                     AccessModifier_owned_ptr _access_modifier,
                                     TypeSpecifier_owned_ptr _type_specifier,
                                     Terminal_owned_ptr _identifier_token,
                                     Terminal_owned_ptr _paren_l_token,
                                     FunctionDefinitionArgList_owned_ptr _function_definition_arg_list,
                                     Terminal_owned_ptr _paren_r_token,
                                     Terminal_owned_ptr _semicolon_token
                                   );
      ~ClassMemberDeclarationMethod();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr identifier_token;
      Terminal_owned_ptr paren_l_token;
      FunctionDefinitionArgList_owned_ptr function_definition_arg_list;
      Terminal_owned_ptr paren_r_token;
      Terminal_owned_ptr semicolon_token;
    };
    class ClassMemberDeclarationConstructor : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationConstructor(
                                        AccessModifier_owned_ptr _access_modifier,
                                        TypeSpecifier_owned_ptr _type_specifier,
                                        Terminal_owned_ptr _paren_l_token,
                                        FunctionDefinitionArgList_owned_ptr _function_definition_arg_list,
                                        Terminal_owned_ptr _paren_r_token,
                                        Terminal_owned_ptr _semicolon_token
                                        );
      ~ClassMemberDeclarationConstructor();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr paren_l_token;
      FunctionDefinitionArgList_owned_ptr function_definition_arg_list;
      Terminal_owned_ptr paren_r_token;
      Terminal_owned_ptr semicolon_token;
    };
    class ClassMemberDeclarationDestructor : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationDestructor(
                                       AccessModifier_owned_ptr _access_modifier,
                                       Terminal_owned_ptr _tilde_token,
                                       TypeSpecifier_owned_ptr _type_specifier,
                                       Terminal_owned_ptr _paren_l_token,
                                       FunctionDefinitionArgList_owned_ptr _function_definition_arg_list,
                                       Terminal_owned_ptr _paren_r_token,
                                       Terminal_owned_ptr _semicolon_token
                                   );
      ~ClassMemberDeclarationDestructor();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      Terminal_owned_ptr tilde_token;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr paren_l_token;
      FunctionDefinitionArgList_owned_ptr function_definition_arg_list;
      Terminal_owned_ptr paren_r_token;
      Terminal_owned_ptr semicolon_token;
    };
    
    class ClassMemberDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        ClassMemberDeclarationVariable_owned_ptr,
        ClassMemberDeclarationMethod_owned_ptr,
        ClassMemberDeclarationConstructor_owned_ptr,
        ClassMemberDeclarationDestructor_owned_ptr,
        ClassDefinition_owned_ptr,
        EnumDefinition_owned_ptr,
        TypeDefinition_owned_ptr
      > MemberType;
      ClassMemberDeclaration(
                             MemberType _member,
                             const JLang::frontend::ast::SyntaxNode & _sn
                             );
      ~ClassMemberDeclaration();
      const ClassMemberDeclaration::MemberType & get_member();
    private:
      MemberType member;
    };
    
    class ClassMemberDeclarationList : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationList();
      ~ClassMemberDeclarationList();
      const std::vector<ClassMemberDeclaration_owned_ptr> & get_members() const;
      void add_member(ClassMemberDeclaration_owned_ptr _member);
    private:
      std::vector<ClassMemberDeclaration_owned_ptr> members;
    };
          
    class ClassDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassDefinition(
                      ClassDeclStart_owned_ptr _class_decl_start,
                      Terminal_owned_ptr _brace_l_token,
                      ClassMemberDeclarationList_owned_ptr _class_member_declaration_list,
                      Terminal_owned_ptr _brace_r_token,
                      Terminal_owned_ptr _semicolon_token
                      );
      ~ClassDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const ClassArgumentList & get_argument_list() const;
      const ClassMemberDeclarationList & get_members() const;
    private:
      ClassDeclStart_owned_ptr class_decl_start;
      Terminal_owned_ptr brace_l_token;
      ClassMemberDeclarationList_owned_ptr class_member_declaration_list;
      Terminal_owned_ptr brace_r_token;
      Terminal_owned_ptr semicolon_token;
    };

    class TypeDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeDefinition(
                     AccessModifier_owned_ptr _access_modifier,
                     Terminal_owned_ptr _typedef_token,
                     TypeSpecifier_owned_ptr _type_specifier,
                     Terminal_owned_ptr _identifier_token,
                     Terminal_owned_ptr _semicolon_token
                     );
      ~TypeDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const TypeSpecifier & get_type_specifier() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      Terminal_owned_ptr typedef_token;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr identifier_token;
      Terminal_owned_ptr semicolon_token;
    };

    class EnumDefinitionValue : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinitionValue(
                          Terminal_owned_ptr _identifier_token,
                          Terminal_owned_ptr _equals_token,
                          ExpressionPrimary_owned_ptr _expression_primary,
                          Terminal_owned_ptr _semicolon_token
                          );                          
      ~EnumDefinitionValue();
      const std::string & get_name() const;
      const ExpressionPrimary & get_expression() const;
    private:
      Terminal_owned_ptr identifier_token;
      Terminal_owned_ptr equals_token;
      ExpressionPrimary_owned_ptr expression_primary;
      Terminal_owned_ptr semicolon_token;      
    };
    
    class EnumDefinitionValueList : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinitionValueList();
      ~EnumDefinitionValueList();
      void add_value(EnumDefinitionValue_owned_ptr);
      const std::vector<EnumDefinitionValue_owned_ptr> &get_values() const;
    private:
      std::vector<EnumDefinitionValue_owned_ptr> values;
    };

    class EnumDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinition(
                     AccessModifier_owned_ptr _access_modifier,
                     Terminal_owned_ptr _enum_token,
                     Terminal_owned_ptr _type_name_token,
                     Terminal_owned_ptr _identifier_token,
                     Terminal_owned_ptr _brace_l_token,
                     EnumDefinitionValueList_owned_ptr _enum_value_list,
                     Terminal_owned_ptr _brace_r_token,
                     Terminal_owned_ptr _semicolon_token
                     );
      ~EnumDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & type_name() const;
      const std::string & enum_name() const;
      const EnumDefinitionValueList & get_value_list() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      Terminal_owned_ptr enum_token;
      Terminal_owned_ptr type_name_token;
      Terminal_owned_ptr identifier_token;
      Terminal_owned_ptr brace_l_token;
      EnumDefinitionValueList_owned_ptr enum_value_list;
      Terminal_owned_ptr brace_r_token;
      Terminal_owned_ptr semicolon_token;
      
    };

    class ExpressionPrimaryIdentifier : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryIdentifier(Terminal_owned_ptr _identifier_token);
      ~ExpressionPrimaryIdentifier();
      const std::string & get_identifier() const;
    private:
      Terminal_owned_ptr identifier_token;
    };
    class ExpressionPrimaryNested : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryNested(
                              Terminal_owned_ptr _paren_l_token,
                              Expression_owned_ptr _expression,
                              Terminal_owned_ptr _paren_r_token
                              );
      ~ExpressionPrimaryNested();
      const Expression & get_expression() const;
    private:
      Terminal_owned_ptr paren_l_token;
      Expression_owned_ptr expression;
      Terminal_owned_ptr paren_r_token;
    };

    class ExpressionPrimaryLiteralInt : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralInt(
                                  Terminal_owned_ptr literal_token
                                  );
      ~ExpressionPrimaryLiteralInt();
      const std::string & get_value() const;
    private:
      Terminal_owned_ptr literal_token;
    };
    class ExpressionPrimaryLiteralChar : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralChar(
                                  Terminal_owned_ptr _literal_token
                                  );
      ~ExpressionPrimaryLiteralChar();
      const std::string & get_value() const;
    private:
      Terminal_owned_ptr literal_token;
    };
    class ExpressionPrimaryLiteralString : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralString(
                                     Terminal_owned_ptr _literal_token
                                     );
      ~ExpressionPrimaryLiteralString();
      const std::string & get_value() const;
    private:
      Terminal_owned_ptr literal_token;
    };
    class ExpressionPrimaryLiteralFloat : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralFloat(
                                  Terminal_owned_ptr _literal_token
                                  );
      ~ExpressionPrimaryLiteralFloat();
      const std::string & get_value() const;
    private:
      Terminal_owned_ptr literal_token;
    };
    
    class ExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<ExpressionPrimaryIdentifier_owned_ptr,
                           ExpressionPrimaryNested_owned_ptr,
                           ExpressionPrimaryLiteralChar_owned_ptr,
                           ExpressionPrimaryLiteralString_owned_ptr,
                           ExpressionPrimaryLiteralInt_owned_ptr,
                           ExpressionPrimaryLiteralFloat_owned_ptr
                           > ExpressionType;
      ExpressionPrimary(ExpressionPrimary::ExpressionType _expression_type, const SyntaxNode & _sn);
      ~ExpressionPrimary();
      const ExpressionPrimary::ExpressionType & get_expression() const;
    private:
      ExpressionPrimary::ExpressionType expression_type;
    };

    class ExpressionPostfixArrayIndex : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixArrayIndex(
                                  Expression_owned_ptr _array_expression,
                                  Terminal_owned_ptr _bracket_l_token,
                                  Expression_owned_ptr _index_expression,
                                  Terminal_owned_ptr _bracket_r_token
                                  );
      ~ExpressionPostfixArrayIndex();
      const Expression & get_array() const;
      const Expression & get_index() const;
    private:
      Expression_owned_ptr array_expression;
      Terminal_owned_ptr bracket_l_token;
      Expression_owned_ptr index_expression;
      Terminal_owned_ptr bracket_r_token;
    };

    class ArgumentExpressionList : public JLang::frontend::ast::SyntaxNode {
    public:
      ArgumentExpressionList();
      ~ArgumentExpressionList();
      const std::vector<Expression_owned_ptr> & get_arguments() const;
      void add_argument(Expression_owned_ptr _argument);
      void add_argument(Terminal_owned_ptr _comma_token, Expression_owned_ptr _argument);
    private:
      std::vector<Terminal_owned_ptr> comma_list;
      std::vector<Expression_owned_ptr> arguments;
    };
    
    class ExpressionPostfixFunctionCall : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixFunctionCall(
                                    Expression_owned_ptr _function_expression,
                                    Terminal_owned_ptr _paren_l_token,
                                    ArgumentExpressionList_owned_ptr _arguments,
                                    Terminal_owned_ptr _paren_r_token
                                    );
      ~ExpressionPostfixFunctionCall();
      const Expression & get_function() const;
      const ArgumentExpressionList & get_arguments() const;
    private:
      Expression_owned_ptr function_expression;
      Terminal_owned_ptr paren_l_token;
      ArgumentExpressionList_owned_ptr arguments;
      Terminal_owned_ptr paren_r_token;
    };

    class ExpressionPostfixDot : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixDot(
                           Expression_owned_ptr _expression,
                           Terminal_owned_ptr _dot_token,
                           Terminal_owned_ptr _identifier_token
                           );
      ~ExpressionPostfixDot();
      const Expression & get_expression() const;
      const std::string & get_identifier() const;
    private:
      Expression_owned_ptr expression;
      Terminal_owned_ptr dot_token;
      Terminal_owned_ptr identifier_token;
    };
    
    class ExpressionPostfixArrow : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixArrow(
                             Expression_owned_ptr _expression,
                             Terminal_owned_ptr _arrow_token,
                             Terminal_owned_ptr _identifier_token
                             );
      ~ExpressionPostfixArrow();
      const Expression & get_expression() const;
      const std::string & get_identifier() const;
    private:
      Expression_owned_ptr expression;
      Terminal_owned_ptr arrow_token;
      Terminal_owned_ptr identifier_token;
    };

    class ExpressionPostfixIncDec : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef enum {
        INCREMENT,
        DECREMENT
      } OperationType;
      ExpressionPostfixIncDec(
                              Expression_owned_ptr _expression,
                              Terminal_owned_ptr _operator_token
                              );
      ~ExpressionPostfixIncDec();
      const ExpressionPostfixIncDec::OperationType & get_type();
      const Expression & get_expression();
    private:
      ExpressionPostfixIncDec::OperationType type;
      Terminal_owned_ptr operator_token;
      Expression_owned_ptr expression;
    };

    class ExpressionUnaryPrefix : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef enum {
        INCREMENT,
        DECREMENT,
        ADDRESSOF,
        DEREFERENCE,
        PLUS,
        MINUS,
        BITWISE_NOT,
        LOGICAL_NOT
      } OperationType;
      ExpressionUnaryPrefix(
                            Terminal_owned_ptr _operator_token,
                            Expression_owned_ptr _expression
                            );
      ~ExpressionUnaryPrefix();
      const ExpressionUnaryPrefix::OperationType & get_type();
      const Expression & get_expression();
    private:
      ExpressionUnaryPrefix::OperationType type;
      Terminal_owned_ptr operator_token;
      Expression_owned_ptr expression;
    };

    class ExpressionUnarySizeofType : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionUnarySizeofType(
                                Terminal_owned_ptr _sizeof_token,
                                Terminal_owned_ptr _paren_l_token,
                                TypeSpecifier_owned_ptr _type_specifier,
                                Terminal_owned_ptr _paren_r_token
                                );
      ~ExpressionUnarySizeofType();
      const TypeSpecifier & get_type_specifier() const;
    private:
      Terminal_owned_ptr sizeof_token;
      Terminal_owned_ptr paren_l_token;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr paren_r_token;
    };

    class ExpressionCast : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionCast(
                     Terminal_owned_ptr _cast_token,
                     Terminal_owned_ptr _paren_l_token,
                     TypeSpecifier_owned_ptr _type_specifier,
                     Terminal_owned_ptr _comma_token,
                     Expression_owned_ptr _expression,
                     Terminal_owned_ptr _paren_r_token
                     );
      ~ExpressionCast();
      const TypeSpecifier & get_type() const;
      const Expression & get_expression() const;
    private:
      Terminal_owned_ptr cast_token;
      Terminal_owned_ptr paren_l_token;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr comma_token;
      Expression_owned_ptr expression;
      Terminal_owned_ptr paren_r_token;
    };

    class ExpressionBinary : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef enum {
        // Arithmetic
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        MODULO,
        // Logical
        LOGICAL_AND,
        LOGICAL_OR,
        LOGICAL_XOR,

        // Bitwise
        BITWISE_AND,
        BITWISE_OR,
        BITWISE_XOR,
        SHIFT_LEFT,
        SHIFT_RIGHT,
        
        // Relational
        COMPARE_LT,
        COMPARE_GT,
        COMPARE_LE,
        COMPARE_GE,
        COMPARE_EQ,
        COMPARE_NE,

        // Assignment
        EQUALS,
        MUL_ASSIGN,
        DIV_ASSIGN,
        MOD_ASSIGN,
        ADD_ASSIGN,
        SUB_ASSIGN,
        LEFT_ASSIGN,
        RIGHT_ASSIGN,
        AND_ASSIGN,
        XOR_ASSIGN,
        OR_ASSIGN        
      } OperationType;
      ExpressionBinary(
                       Expression_owned_ptr _expression_a,
                       Terminal_owned_ptr _operator_token,
                       Expression_owned_ptr _expression_b
                       );
      ~ExpressionBinary();
      const Expression & get_a() const;
      const ExpressionBinary::OperationType & get_operator() const;
      const Expression & get_b() const;
      
    private:
      OperationType type;
      Expression_owned_ptr expression_a;
      Terminal_owned_ptr operator_token;
      Expression_owned_ptr expression_b;
    };
    
    class ExpressionTrinary : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionTrinary(
                        Expression_owned_ptr _condition,
                        Terminal_owned_ptr _questionmark_token,
                        Expression_owned_ptr _if_expression,
                        Terminal_owned_ptr _colon_token,
                        Expression_owned_ptr _else_expression
                        );
      ~ExpressionTrinary();
      const Expression & get_condition() const;
      const Expression & get_if() const;
      const Expression & get_else() const;
    private:
      Expression_owned_ptr condition;
      Terminal_owned_ptr questionmark_token;
      Expression_owned_ptr if_expression;
      Terminal_owned_ptr colon_token;
      Expression_owned_ptr else_expression;      
    };
    
    class Expression : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<ExpressionPrimary_owned_ptr,
                           ExpressionPostfixArrayIndex_owned_ptr,
                           ExpressionPostfixFunctionCall_owned_ptr,
                           ExpressionPostfixDot_owned_ptr,
                           ExpressionPostfixArrow_owned_ptr,
                           ExpressionPostfixIncDec_owned_ptr,
                           ExpressionUnaryPrefix_owned_ptr,
                           ExpressionUnarySizeofType_owned_ptr,
                           ExpressionBinary_owned_ptr,
                           ExpressionTrinary_owned_ptr,
                           ExpressionCast_owned_ptr
                           > ExpressionType;
      Expression(Expression::ExpressionType _expression_type, const SyntaxNode & _sn);
      ~Expression();
      const Expression::ExpressionType & get_expression() const;
    private:
      Expression::ExpressionType expression_type;
    };
    
    class GlobalInitializerExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerExpressionPrimary(Terminal_owned_ptr _equals_token,
                                         ExpressionPrimary_owned_ptr _expression
                                         );
      ~GlobalInitializerExpressionPrimary();
      const ExpressionPrimary & get_expression() const;
    private:
      Terminal_owned_ptr equals_token;
      ExpressionPrimary_owned_ptr expression;

    };
    class GlobalInitializerAddressofExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerAddressofExpressionPrimary(
                                                  Terminal_owned_ptr _equals_token,
                                                  Terminal_owned_ptr _addressof_token,
                                                  ExpressionPrimary_owned_ptr _expression
                                         );
      ~GlobalInitializerAddressofExpressionPrimary();
      const ExpressionPrimary & get_expression() const;
    private:
      Terminal_owned_ptr equals_token;
      Terminal_owned_ptr addressof_token;
      ExpressionPrimary_owned_ptr expression;
    };

    class StructInitializer : public JLang::frontend::ast::SyntaxNode {
    public:
      StructInitializer(
                        Terminal_owned_ptr _dot_token,
                        Terminal_owned_ptr _identifier_token,
                        GlobalInitializer_owned_ptr _global_initializer,
                        Terminal_owned_ptr _semicolon_token
                        );
      ~StructInitializer();
      const GlobalInitializer & get_initializer() const;
    private:
      Terminal_owned_ptr dot_token;
      Terminal_owned_ptr identifier_token;
      GlobalInitializer_owned_ptr global_initializer;
      Terminal_owned_ptr semicolon_token;
    };
          
    class StructInitializerList : public JLang::frontend::ast::SyntaxNode {
    public:
      StructInitializerList();
      ~StructInitializerList();
      void add_initializer(StructInitializer_owned_ptr initializer);
      const std::vector<StructInitializer_owned_ptr> & get_initializers() const;
    private:
      std::vector<StructInitializer_owned_ptr> initializers;
    };
          
    class GlobalInitializerStructInitializerList : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerStructInitializerList(
                                             Terminal_owned_ptr _equals_token,
                                             Terminal_owned_ptr _brace_l_token,
                                             StructInitializerList_owned_ptr _struct_initializer,
                                             Terminal_owned_ptr _brace_r_token
                                             );

      ~GlobalInitializerStructInitializerList();
      const StructInitializerList & get_struct_initializer() const;
    private:
      Terminal_owned_ptr equals_token;
      Terminal_owned_ptr brace_l_token;
      StructInitializerList_owned_ptr struct_initializer;
      Terminal_owned_ptr brace_r_token;
    };
    
    class GlobalInitializer : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        GlobalInitializerExpressionPrimary_owned_ptr,
        GlobalInitializerAddressofExpressionPrimary_owned_ptr,
        GlobalInitializerStructInitializerList_owned_ptr,
        nullptr_t> GlobalInitializerType;
      GlobalInitializer();
      GlobalInitializer(GlobalInitializerType initializer, const JLang::frontend::ast::SyntaxNode & _sn);
      ~GlobalInitializer();
      const GlobalInitializerType & get_initializer() const;
    private:
      GlobalInitializerType initializer;
    };
    
    class FileStatementGlobalDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementGlobalDefinition(
                                    AccessModifier_owned_ptr _access_modifier,
                                    UnsafeModifier_owned_ptr _unsafe_modifier,
                                    TypeSpecifier_owned_ptr _type_specifier,
                                    Terminal_owned_ptr _name,
                                    ArrayLength_owned_ptr _array_length,
                                    GlobalInitializer_owned_ptr _global_initializer,
                                    Terminal_owned_ptr _semicolon
                                    );
      ~FileStatementGlobalDefinition();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const ArrayLength & get_array_length() const;
      const GlobalInitializer & get_global_initializer() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      UnsafeModifier_owned_ptr unsafe_modifier;
      TypeSpecifier_owned_ptr type_specifier;
      Terminal_owned_ptr name; // function name (IDENTIFIER)
      ArrayLength_owned_ptr array_length;
      GlobalInitializer_owned_ptr global_initializer;
      Terminal_owned_ptr semicolon;
    };
    
    class NamespaceDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      NamespaceDeclaration(
                           AccessModifier_owned_ptr _access_modifier,
                           Terminal_owned_ptr _namespace_token,
                           Terminal_owned_ptr _identifier_token
                           );
      ~NamespaceDeclaration();
      const AccessModifier & get_access_modifier() const;
      const Terminal & get_name() const;
    private:
      AccessModifier_owned_ptr access_modifier;
      Terminal_owned_ptr namespace_token;
      Terminal_owned_ptr identifier_token;
    };

    class FileStatementNamespace : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementNamespace(NamespaceDeclaration_owned_ptr _namespace_declaration,
                             Terminal_owned_ptr _brace_l_token,
                             FileStatementList_owned_ptr _file_statement_list,
                             Terminal_owned_ptr _brace_r_token,
                             Terminal_owned_ptr _semicolon_token
                             );
      ~FileStatementNamespace();
      const NamespaceDeclaration & get_declaration() const;
      const FileStatementList & get_statement_list() const;
    private:
      NamespaceDeclaration_owned_ptr namespace_declaration;
      Terminal_owned_ptr brace_l_token;
      FileStatementList_owned_ptr file_statement_list;
      Terminal_owned_ptr brace_r_token;
      Terminal_owned_ptr semicolon_token;
    };

    class UsingAs : public JLang::frontend::ast::SyntaxNode {
    public:
      UsingAs(
              Terminal_owned_ptr _as_token,
              Terminal_owned_ptr _identifier_token
              );
      UsingAs();
      ~UsingAs();
      const std::string & get_using_name() const;
    private:
      std::string using_name;
      Terminal_owned_ptr as_token;
      Terminal_owned_ptr identifier_token;
    };
    
    class FileStatementUsing : public JLang::frontend::ast::SyntaxNode {
    public:
          FileStatementUsing(AccessModifier_owned_ptr _access_modifier,
                             Terminal_owned_ptr _using,
                             Terminal_owned_ptr _namespace,
                             Terminal_owned_ptr _namespace_name,
                             UsingAs_owned_ptr _using_as,
                             Terminal_owned_ptr _semicolon);
      ~FileStatementUsing();
    private:
      const AccessModifier & get_access_modifier() const;
      std::string & get_namespace() const;
      const UsingAs &get_using_as() const;
      
      AccessModifier_owned_ptr access_modifier;
      Terminal_owned_ptr using_token;
      Terminal_owned_ptr namespace_token;
      Terminal_owned_ptr namespace_name_token;
      UsingAs_owned_ptr using_as;
      Terminal_owned_ptr semicolon_token;
    };

    /**
     * This class represents a list of statements at the file-level.
     * These statements are things like global variable declarations,
     * function declarations, function definitions, and type definitions
     * that appear at the top-level of the translation unit or
     * possibly nested inside namespaces at the translation unit level.
     */
    class FileStatementList : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementList();
      FileStatementList(Terminal_owned_ptr _yyeof);
      ~FileStatementList();
      const std::vector<FileStatement_owned_ptr> & get_statements() const;
      void add_statement(FileStatement_owned_ptr statement);
    private:
      Terminal_owned_ptr yyeof;
      std::vector<FileStatement_owned_ptr> statements;
    };
    
    /**
     * This class represents a translation unit, the top-level
     * result of parsing a source-file.  All syntax information
     * about the file appears in a tree-like structure beneath
     * this node.
     */
    class TranslationUnit : public JLang::frontend::ast::SyntaxNode {
    public:
      
      TranslationUnit(
                      FileStatementList_owned_ptr file_statement_list,
                      Terminal_owned_ptr yyeof_token
                      );
      ~TranslationUnit();
      const FileStatementList&  get_statements() const;
    private:
      Terminal_owned_ptr yyeof_token;
      FileStatementList_owned_ptr file_statement_list;
    };
    
};

/*! @} End of Doxygen Groups*/
