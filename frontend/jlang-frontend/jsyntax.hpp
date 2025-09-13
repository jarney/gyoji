#pragma once

#include <list>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include "ast.hpp"
#include "namespace.hpp"

namespace JLang::frontend {
  typedef struct return_data_st {
    ASTNode::ptr translation_unit;
    NamespaceContext namespace_context;
  } return_data_t;

  namespace alt_imp {
  
  class Terminal;

  class AccessModifier;
  class UnsafeModifier;
  class TypeSpecifier;
    class FunctionDefinitionArg;
  class FunctionDefinitionArgList;

    class ClassDeclStart;
    class ClassArgumentList;
    class ClassDeclarationMemberList;
  class ClassDefinition;
    
  class TypeDefinition;

    class EnumDefinitionValue;
    class EnumDefinitionValueList;
  class EnumDefinition;

  class ExpressionPrimary;

      class StatementBlock;
      class StatementIfElse;
      class StatementWhile;
      class StatementFor;
      class StatementSwitch;
      class StatementReturn;
      class StatementContinue;
      class StatementGoto;
      class StatementBreak;
      class StatementLabel;
      class StatementExpression;
      class StatementVariableDeclaration;
    class Statement;
  class StatementList;
    
  class TranslationUnit;

        class ScopeBody;
      class FileStatementFunctionDefinition;
    
      class FileStatementFunctionDeclaration;
    
        class ArrayLength;
          class GlobalInitializerExpressionPrimary;
          class GlobalInitializerAddressofExpressionPrimary;
              class StructInitializer;
            class StructInitializerList;
          class GlobalInitializerStructInitializerList;
        class GlobalInitializer;
      class FileStatementGlobalDefinition;
      class FileStatementClassDefinition;
      class FileStatementEnumDefinition;
      class FileStatementTypeDefinition;
    
        class NamespaceDeclaration;
      class FileStatementNamespace;
        class UsingAs;
      class FileStatementUsing;
      class FileStatement;
    class FileStatementList;

    class SyntaxNode {
    public:
      typedef std::variant<
        Terminal*,
      
        AccessModifier*,
        UnsafeModifier*,
        TypeSpecifier*,
          FunctionDefinitionArg*,
        FunctionDefinitionArgList*,

          ClassDeclStart*,
          ClassArgumentList*,
          ClassDeclarationMemberList*,
        ClassDefinition*,

        TypeDefinition*,

            EnumDefinitionValue*,
          EnumDefinitionValueList*,
        EnumDefinition*,
      
        ExpressionPrimary *,

            StatementBlock*,
            StatementIfElse*,
            StatementWhile*,
            StatementFor*,
            StatementSwitch*,
            StatementReturn*,
            StatementContinue*,
            StatementGoto*,
            StatementBreak*,
            StatementLabel*,
            StatementExpression*,
            StatementVariableDeclaration*,
          Statement*,
        StatementList*,
      
        TranslationUnit*,
              ScopeBody *,
            FileStatementFunctionDefinition*,
            FileStatementFunctionDeclaration*,
              ArrayLength*,
      
                GlobalInitializerExpressionPrimary*,
                GlobalInitializerAddressofExpressionPrimary*,
                    StructInitializer*,
                  StructInitializerList*,
                GlobalInitializerStructInitializerList*,
              GlobalInitializer*,
            FileStatementGlobalDefinition*,
            FileStatementClassDefinition*,
            FileStatementEnumDefinition*,
            FileStatementTypeDefinition*,
              NamespaceDeclaration*,
            FileStatementNamespace*,
              UsingAs*,
            FileStatementUsing*,
            FileStatement*,
          FileStatementList*
      > specific_type_t;
      
      SyntaxNode(std::string _type, specific_type_t _data);
      ~SyntaxNode();
      
      // Access to the child list should be
      // immutable because this is really
      // just a view over the list.
      const std::vector<const SyntaxNode*> & get_children() const;
      const std::string & get_type() const;
      const SyntaxNode::specific_type_t &get_data() const;
      
    private:
      // This list does NOT own its children, so
      // the class deriving from this one must
      // agree to own the pointers separately.
      std::vector<const SyntaxNode*> children; 
      
    protected:
      // Children are owned by their parents, so this is
      // private and can only be called by the
      // deriving class.
      void add_child(const SyntaxNode* node);
      void prepend_child(const SyntaxNode* node);
      
      std::string type;
      specific_type_t data;
    };
    
    template <class T> class PtrProtocol {
    public:
      typedef T *raw_ptr;
      typedef std::unique_ptr<T> owned_ptr;
    };
    

    enum TerminalNonSyntaxType {
      EXTRA_COMMENT_SINGLE_LINE,
      EXTRA_COMMENT_MULTI_LINE,
      EXTRA_WHITESPACE,
      EXTRA_FILE_METADATA
    };
    
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
    class TerminalNonSyntax : public PtrProtocol<TerminalNonSyntax> {
    public:
      TerminalNonSyntax(TerminalNonSyntaxType _type, std::string _data);
      ~TerminalNonSyntax();

      /**
       * This method returns the type of data
       * from the enum above.
       */
      const TerminalNonSyntaxType &get_type() const;
      /**
       * This method provides access to the raw input
       * for the type of non-syntax data available.
       */
      const std::string & get_data() const;
      
    private:
      TerminalNonSyntaxType type;
      std::string data;
    };

    /**
     * Terminals are the raw tokens received by the lexer.
     * They carry the token information as well as any
     * "Non-syntax" data like comments and whitespace.
     */
    class Terminal : public SyntaxNode, public PtrProtocol<Terminal> {
    public:
      //      typedef Terminal *raw_ptr;
      //      typedef std::unique_ptr<Terminal> owned_ptr;
      Terminal();
      ~Terminal();
      int type;
      std::string typestr;
      std::string value;
      size_t lineno;
      size_t colno;
      std::string fully_qualified_name;
      std::vector<TerminalNonSyntax::raw_ptr> get_whitespace();
      
      // The terminal "owns" uniquely all of the non-syntax data
      // in this vector.  It may be returned to access it,
      // but these are owned pointers, so they must only
      // be de-referenced and never assigned to
      std::vector<TerminalNonSyntax::owned_ptr> non_syntax;
    };


    class FileStatement : public SyntaxNode, public PtrProtocol<FileStatement> {
    public:
      typedef std::variant<
        std::unique_ptr<FileStatementFunctionDefinition>,
        std::unique_ptr<FileStatementFunctionDeclaration>,
        std::unique_ptr<FileStatementGlobalDefinition>,
        std::unique_ptr<FileStatementClassDefinition>,
        std::unique_ptr<FileStatementEnumDefinition>,
        std::unique_ptr<FileStatementTypeDefinition>,
        std::unique_ptr<FileStatementNamespace>,
        std::unique_ptr<FileStatementUsing>> FileStatementType;

      FileStatement(FileStatementType _statement, SyntaxNode *raw_ptr);
      ~FileStatement();

      const FileStatementType & get_statement() const;

    private:
      FileStatementType statement;
    };

    class AccessModifier : public SyntaxNode, public PtrProtocol<AccessModifier> {
    public:
      typedef enum {
        PUBLIC,
        PROTECTED,
        PRIVATE
      } AccessModifierType;
      AccessModifier(Terminal::owned_ptr _modifier, AccessModifierType _type);
      ~AccessModifier();
      const AccessModifierType & get_type() const;
    private:
      AccessModifierType type;
      Terminal::owned_ptr modifier;
    };
    class UnsafeModifier : public SyntaxNode, public PtrProtocol<UnsafeModifier> {
    public:
      UnsafeModifier(Terminal::owned_ptr _unsafe_token);
      UnsafeModifier();
      ~UnsafeModifier();
      bool is_unsafe() const;
    private:
      Terminal::owned_ptr unsafe_token;
      
    };

    class TypeSpecifier : public SyntaxNode, public PtrProtocol<TypeSpecifier> {
    public:
      TypeSpecifier();
      ~TypeSpecifier();
    };

    class FunctionDefinitionArg : public SyntaxNode, public PtrProtocol<FunctionDefinitionArg> {
    public:
      FunctionDefinitionArg(TypeSpecifier::owned_ptr _type_specifier,
                            Terminal::owned_ptr _identifier_token
                            );
      ~FunctionDefinitionArg();
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
    private:
      TypeSpecifier::owned_ptr type_specifier;
      Terminal::owned_ptr identifier_token;
    };
    class FunctionDefinitionArgList : public SyntaxNode, public PtrProtocol<FunctionDefinitionArgList> {
    public:
      FunctionDefinitionArgList();
      ~FunctionDefinitionArgList();
      const std::vector<FunctionDefinitionArg::owned_ptr> & get_arguments() const;
      void add_argument(FunctionDefinitionArg::owned_ptr _argument);
    private:
      std::vector<FunctionDefinitionArg::owned_ptr> arguments;
    };

    class FileStatementFunctionDeclaration : public SyntaxNode, public PtrProtocol<FileStatementFunctionDeclaration> {
    public:
      FileStatementFunctionDeclaration(
                                      AccessModifier::owned_ptr _access_modifier,
                                      UnsafeModifier::owned_ptr _unsafe_modifier,
                                      TypeSpecifier::owned_ptr _type_specifier,
                                      Terminal::owned_ptr _name,
                                      Terminal::owned_ptr _paren_l,
                                      FunctionDefinitionArgList::owned_ptr _arguments,
                                      Terminal::owned_ptr _paren_r,
                                      Terminal::owned_ptr _semicolon
                                      );
      ~FileStatementFunctionDeclaration();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
      
    private:
      AccessModifier::owned_ptr access_modifier;
      UnsafeModifier::owned_ptr unsafe_modifier;
      TypeSpecifier::owned_ptr type_specifier;
      Terminal::owned_ptr name; // function name (IDENTIFIER)
      Terminal::owned_ptr paren_l; // argument list delimiter PAREN_L
      FunctionDefinitionArgList::owned_ptr arguments;
      Terminal::owned_ptr paren_r; // argument list delimiter PAREN_R
      Terminal::owned_ptr semicolon; // argument list delimiter SEMICOLON
    };


    class StatementBlock : public SyntaxNode, public PtrProtocol<StatementBlock> {
    public:
      StatementBlock();
      ~StatementBlock();
    private:
    };
    class StatementIfElse : public SyntaxNode, public PtrProtocol<StatementIfElse> {
    public:
      StatementIfElse();
      ~StatementIfElse();
    private:
    };
    class StatementWhile : public SyntaxNode, public PtrProtocol<StatementWhile> {
    public:
      StatementWhile();
      ~StatementWhile();
    private:
    };
    class StatementFor : public SyntaxNode, public PtrProtocol<StatementFor> {
    public:
      StatementFor();
      ~StatementFor();
    private:
    };
    class StatementSwitch : public SyntaxNode, public PtrProtocol<StatementSwitch> {
    public:
      StatementSwitch();
      ~StatementSwitch();
    private:
    };
    class StatementReturn : public SyntaxNode, public PtrProtocol<StatementReturn> {
    public:
      StatementReturn();
      ~StatementReturn();
    private:
    };
    class StatementContinue : public SyntaxNode, public PtrProtocol<StatementContinue> {
    public:
      StatementContinue();
      ~StatementContinue();
    private:
    };
    class StatementGoto : public SyntaxNode, public PtrProtocol<StatementGoto> {
    public:
      StatementGoto();
      ~StatementGoto();
    private:
    };
    class StatementBreak : public SyntaxNode, public PtrProtocol<StatementBreak> {
    public:
      StatementBreak();
      ~StatementBreak();
    private:
    };
    class StatementLabel : public SyntaxNode, public PtrProtocol<StatementLabel> {
    public:
      StatementLabel();
      ~StatementLabel();
    private:
    };
    class StatementExpression : public SyntaxNode, public PtrProtocol<StatementExpression> {
    public:
      StatementExpression();
      ~StatementExpression();
    private:
    };
    class StatementVariableDeclaration : public SyntaxNode, public PtrProtocol<StatementVariableDeclaration> {
    public:
      StatementVariableDeclaration();
      ~StatementVariableDeclaration();
    private:
    };
    
    class Statement : public SyntaxNode, public PtrProtocol<StatementList> {
    public:
      typedef std::variant<
            StatementBlock::owned_ptr,
            StatementIfElse::owned_ptr,
            StatementWhile::owned_ptr,
            StatementFor::owned_ptr,
            StatementSwitch::owned_ptr,
            StatementReturn::owned_ptr,
            StatementContinue::owned_ptr,
            StatementGoto::owned_ptr,
            StatementBreak::owned_ptr,
            StatementLabel::owned_ptr,
            StatementExpression::owned_ptr,
            StatementVariableDeclaration::owned_ptr
      > StatementType;

      Statement(StatementType _statement);
      ~Statement();
      const StatementType & get_statement() const;
    private:
      StatementType statement;
    };
    
    class StatementList : public SyntaxNode, public PtrProtocol<StatementList> {
    public:
      StatementList();
      ~StatementList();
      void add_statement(Statement::owned_ptr _statement);
      const std::vector<Statement::owned_ptr> &get_statements() const;
    private:
      std::vector<Statement::owned_ptr> statements;
    };
    
    class ScopeBody : public SyntaxNode, public PtrProtocol<ScopeBody> {
    public:
      ScopeBody();
      ~ScopeBody();
    };
    
    class FileStatementFunctionDefinition : public SyntaxNode, public PtrProtocol<FileStatementFunctionDefinition> {
    public:
      FileStatementFunctionDefinition(
                                      AccessModifier::owned_ptr _access_modifier,
                                      UnsafeModifier::owned_ptr _unsafe_modifier,
                                      TypeSpecifier::owned_ptr _type_specifier,
                                      Terminal::owned_ptr _name,
                                      Terminal::owned_ptr _paren_l,
                                      FunctionDefinitionArgList::owned_ptr _arguments,
                                      Terminal::owned_ptr _paren_r,
                                      ScopeBody::owned_ptr _scope_body
                                      );
      ~FileStatementFunctionDefinition();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
      const ScopeBody & get_scope_body() const;
      
    private:
      AccessModifier::owned_ptr access_modifier;
      UnsafeModifier::owned_ptr unsafe_modifier;
      TypeSpecifier::owned_ptr type_specifier;
      Terminal::owned_ptr name; // function name (IDENTIFIER)
      Terminal::owned_ptr paren_l; // argument list delimiter PAREN_L
      FunctionDefinitionArgList::owned_ptr arguments;
      Terminal::owned_ptr paren_r; // argument list delimiter PAREN_R
      ScopeBody::owned_ptr scope_body; // argument list delimiter SEMICOLON
    };

    class ArrayLength : public SyntaxNode, public PtrProtocol<ArrayLength> {
    public:
      ArrayLength();
      ArrayLength(
                  Terminal::owned_ptr _bracket_l_token,
                  Terminal::owned_ptr _literal_int_token,
                  Terminal::owned_ptr _bracket_r_token
                  );
      ~ArrayLength();
      bool is_array() const;
      size_t get_size() const;
    private:
      Terminal::owned_ptr bracket_l_token;
      Terminal::owned_ptr literal_int_token;
      Terminal::owned_ptr bracket_r_token;
    };

    class ClassDeclStart : public SyntaxNode, public PtrProtocol<ClassDeclStart> {
    public:
      ClassDeclStart(
                     AccessModifier::owned_ptr _access_modifier,
                     Terminal::owned_ptr _class_token,
                     Terminal::owned_ptr _identifier_token,
                     std::unique_ptr<ClassArgumentList> _class_argument_list
                     );
                     
      ~ClassDeclStart();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const ClassArgumentList & get_argument_list() const;
    private:
      AccessModifier::owned_ptr access_modifier;
      Terminal::owned_ptr class_token;
      Terminal::owned_ptr identifier_token;
      std::unique_ptr<ClassArgumentList> class_argument_list;
    };
    class ClassArgumentList : public SyntaxNode, public PtrProtocol<ClassArgumentList> {
    public:
      ClassArgumentList(Terminal::owned_ptr _argument);
      ~ClassArgumentList();
      void add_argument(Terminal::owned_ptr _comma, Terminal::owned_ptr _argument);
      void add_parens(Terminal::owned_ptr _paren_l, Terminal::owned_ptr _paren_r);
      const std::vector<Terminal::owned_ptr> & get_arguments() const;
    private:
      Terminal::owned_ptr paren_l;
      std::vector<Terminal::owned_ptr> comma_list;
      std::vector<Terminal::owned_ptr> argument_list;
      Terminal::owned_ptr paren_r;
    };

    class ClassDeclarationMemberList : public SyntaxNode, public PtrProtocol<ClassDeclarationMemberList> {
    public:
      ClassDeclarationMemberList();
      ~ClassDeclarationMemberList();
    private:
    };
          
    class ClassDefinition : public SyntaxNode, public PtrProtocol<ClassDefinition> {
    public:
      ClassDefinition(
                      ClassDeclStart::owned_ptr _class_decl_start,
                      Terminal::owned_ptr _brace_l_token,
                      ClassDeclarationMemberList::owned_ptr _class_declaration_member_list,
                      Terminal::owned_ptr _brace_r_token,
                      Terminal::owned_ptr _semicolon_token
                      );
      ~ClassDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const ClassArgumentList & get_argument_list() const;
      const ClassDeclarationMemberList & get_members() const;
    private:
      ClassDeclStart::owned_ptr class_decl_start;
      Terminal::owned_ptr brace_l_token;
      ClassDeclarationMemberList::owned_ptr class_declaration_member_list;
      Terminal::owned_ptr brace_r_token;
      Terminal::owned_ptr semicolon_token;
    };

    class TypeDefinition : public SyntaxNode, public PtrProtocol<TypeDefinition> {
    public:
      TypeDefinition(
                     AccessModifier::owned_ptr _access_modifier,
                     Terminal::owned_ptr _typedef_token,
                     TypeSpecifier::owned_ptr _type_specifier,
                     Terminal::owned_ptr _identifier_token,
                     Terminal::owned_ptr _semicolon_token
                     );
      ~TypeDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const TypeSpecifier & get_type_specifier() const;
    private:
      AccessModifier::owned_ptr access_modifier;
      Terminal::owned_ptr typedef_token;
      TypeSpecifier::owned_ptr type_specifier;
      Terminal::owned_ptr identifier_token;
      Terminal::owned_ptr semicolon_token;
    };

    class EnumDefinitionValue : public SyntaxNode, public PtrProtocol<EnumDefinitionValue> {
    public:
      EnumDefinitionValue(
                          Terminal::owned_ptr _identifier_token,
                          Terminal::owned_ptr _equals_token,
                          std::unique_ptr<ExpressionPrimary> _expression_primary,
                          Terminal::owned_ptr _semicolon_token
                          );                          
      ~EnumDefinitionValue();
      const std::string & get_name() const;
      const ExpressionPrimary & get_expression() const;
    private:
      Terminal::owned_ptr identifier_token;
      Terminal::owned_ptr equals_token;
      std::unique_ptr<ExpressionPrimary> expression_primary;
      Terminal::owned_ptr semicolon_token;
    };
    
    class EnumDefinitionValueList : public SyntaxNode, public PtrProtocol<EnumDefinitionValueList> {
    public:
      EnumDefinitionValueList();
      ~EnumDefinitionValueList();
      void add_value(EnumDefinitionValue::owned_ptr);
      const std::vector<EnumDefinitionValue::owned_ptr> &get_values() const;
    private:
      std::vector<EnumDefinitionValue::owned_ptr> values;
    };

    class EnumDefinition : public SyntaxNode, public PtrProtocol<EnumDefinition> {
    public:
      EnumDefinition(
                     AccessModifier::owned_ptr _access_modifier,
                     Terminal::owned_ptr _enum_token,
                     Terminal::owned_ptr _type_name_token,
                     Terminal::owned_ptr _identifier_token,
                     Terminal::owned_ptr _brace_l_token,
                     EnumDefinitionValueList::owned_ptr _enum_value_list,
                     Terminal::owned_ptr _brace_r_token,
                     Terminal::owned_ptr _semicolon_token
                     );
      ~EnumDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & type_name() const;
      const std::string & enum_name() const;
      const EnumDefinitionValueList & get_value_list() const;
    private:
      AccessModifier::owned_ptr access_modifier;
      Terminal::owned_ptr enum_token;
      Terminal::owned_ptr type_name_token;
      Terminal::owned_ptr identifier_token;
      Terminal::owned_ptr brace_l_token;
      EnumDefinitionValueList::owned_ptr enum_value_list;
      Terminal::owned_ptr brace_r_token;
      Terminal::owned_ptr semicolon_token;
      
    };
    
    class ExpressionPrimary : public SyntaxNode, public PtrProtocol<ExpressionPrimary> {
    public:
      ExpressionPrimary();
      ~ExpressionPrimary();
    };
    
    class GlobalInitializerExpressionPrimary : public SyntaxNode, public PtrProtocol<GlobalInitializerExpressionPrimary> {
    public:
      GlobalInitializerExpressionPrimary(Terminal::owned_ptr _equals_token,
                                         ExpressionPrimary::owned_ptr _expression
                                         );
      ~GlobalInitializerExpressionPrimary();
      const ExpressionPrimary & get_expression() const;
    private:
      Terminal::owned_ptr equals_token;
      ExpressionPrimary::owned_ptr expression;

    };
    class GlobalInitializerAddressofExpressionPrimary : public SyntaxNode, public PtrProtocol<GlobalInitializerExpressionPrimary> {
      GlobalInitializerAddressofExpressionPrimary(
                                                  Terminal::owned_ptr _equals_token,
                                                  Terminal::owned_ptr _addressof_token,
                                                  ExpressionPrimary::owned_ptr _expression
                                         );
      ~GlobalInitializerAddressofExpressionPrimary();
      const ExpressionPrimary & get_expression() const;
    private:
      Terminal::owned_ptr equals_token;
      Terminal::owned_ptr addressof_token;
      ExpressionPrimary::owned_ptr expression;
    };

    class StructInitializer : public SyntaxNode, public PtrProtocol<StructInitializer> {
    public:
      StructInitializer(
                        Terminal::owned_ptr _dot_token,
                        Terminal::owned_ptr _identifier_token,
                        std::unique_ptr<GlobalInitializer> _global_initializer,
                        Terminal::owned_ptr _semicolon_token
                        );
      ~StructInitializer();
      const GlobalInitializer & get_initializer() const;
    private:
      Terminal::owned_ptr dot_token;
      Terminal::owned_ptr identifier_token;
      std::unique_ptr<GlobalInitializer> global_initializer;
      Terminal::owned_ptr semicolon_token;
    };
          
    class StructInitializerList : public SyntaxNode, public PtrProtocol<StructInitializerList> {
    public:
      StructInitializerList();
      ~StructInitializerList();
      void add_initializer(StructInitializer::owned_ptr initializer);
      const std::vector<StructInitializer::owned_ptr> & get_initializers() const;
    private:
      std::vector<StructInitializer::owned_ptr> initializers;
    };
          
    class GlobalInitializerStructInitializerList : public SyntaxNode, public PtrProtocol<GlobalInitializerExpressionPrimary> {
    public:
      GlobalInitializerStructInitializerList(
                                             Terminal::owned_ptr _equals_token,
                                             Terminal::owned_ptr _brace_l_token,
                                             StructInitializerList::owned_ptr _struct_initializer,
                                             Terminal::owned_ptr _brace_r_token
                                             );

      ~GlobalInitializerStructInitializerList();
      const StructInitializerList & get_struct_initializer() const;
    private:
      Terminal::owned_ptr equals_token;
      Terminal::owned_ptr brace_l_token;
      StructInitializerList::owned_ptr struct_initializer;
      Terminal::owned_ptr brace_r_token;
    };
    
    class GlobalInitializer : public SyntaxNode, public PtrProtocol<GlobalInitializer> {
    public:
      typedef std::variant<
        GlobalInitializerExpressionPrimary::owned_ptr,
        GlobalInitializerAddressofExpressionPrimary::owned_ptr,
        GlobalInitializerStructInitializerList::owned_ptr> GlobalInitializerType;
      GlobalInitializer(GlobalInitializerType initializer, SyntaxNode *raw_ptr);
      ~GlobalInitializer();
      const GlobalInitializerType & get_initializer() const;
    private:
      GlobalInitializerType initializer;
    };
    
    class FileStatementGlobalDefinition : public SyntaxNode, public PtrProtocol<FileStatementGlobalDefinition> {
    public:
      FileStatementGlobalDefinition(
                                    AccessModifier::owned_ptr _access_modifier,
                                    UnsafeModifier::owned_ptr _unsafe_modifier,
                                    TypeSpecifier::owned_ptr _type_specifier,
                                    Terminal::owned_ptr _name,
                                    ArrayLength::owned_ptr _array_length,
                                    GlobalInitializer::owned_ptr _global_initializer,
                                    Terminal::owned_ptr _semicolon
                                    );
      ~FileStatementGlobalDefinition();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const ArrayLength & get_array_length() const;
      const GlobalInitializer & get_global_initializer() const;
    private:
      AccessModifier::owned_ptr access_modifier;
      UnsafeModifier::owned_ptr unsafe_modifier;
      TypeSpecifier::owned_ptr type_specifier;
      Terminal::owned_ptr name; // function name (IDENTIFIER)
      ArrayLength::owned_ptr array_length;
      GlobalInitializer::owned_ptr global_initializer;
      Terminal::owned_ptr semicolon;
    };
    
    class FileStatementClassDefinition : public SyntaxNode, public PtrProtocol<FileStatementClassDefinition> {
    public:
    private:
    };
    class FileStatementEnumDefinition : public SyntaxNode, public PtrProtocol<FileStatementEnumDefinition> {
    public:
    private:
    };
    class FileStatementTypeDefinition : public SyntaxNode, public PtrProtocol<FileStatementTypeDefinition> {
    public:
    private:
    };
    class NamespaceDeclaration : public SyntaxNode, public PtrProtocol<NamespaceDeclaration> {
    public:
      NamespaceDeclaration(
                           AccessModifier::owned_ptr _access_modifier,
                           Terminal::owned_ptr _namespace_token,
                           Terminal::owned_ptr _identifier_token
                           );
      ~NamespaceDeclaration();
      const AccessModifier & get_access_modifier() const;
      const Terminal & get_name() const;
    private:
      AccessModifier::owned_ptr access_modifier;
      Terminal::owned_ptr namespace_token;
      Terminal::owned_ptr identifier_token;
    };

    class FileStatementNamespace : public SyntaxNode, public PtrProtocol<FileStatementNamespace> {
    public:
      FileStatementNamespace(NamespaceDeclaration::owned_ptr _namespace_declaration,
                             Terminal::owned_ptr _brace_l_token,
                             std::unique_ptr<FileStatementList> _file_statement_list,
                             Terminal::owned_ptr _brace_r_token,
                             Terminal::owned_ptr _semicolon_token
                             );
      ~FileStatementNamespace();
      const NamespaceDeclaration & get_declaration() const;
      const FileStatementList & get_statement_list() const;
    private:
      NamespaceDeclaration::owned_ptr namespace_declaration;
      Terminal::owned_ptr brace_l_token;
      std::unique_ptr<FileStatementList> file_statement_list;
      Terminal::owned_ptr brace_r_token;
      Terminal::owned_ptr semicolon_token;
    };

    class UsingAs : public SyntaxNode, public PtrProtocol<UsingAs> {
    public:
      UsingAs(
              Terminal::owned_ptr _as_token,
              Terminal::owned_ptr _identifier_token
              );
      UsingAs();
      ~UsingAs();
      const std::string & get_using_name() const;
    private:
      std::string using_name;
      Terminal::owned_ptr as_token;
      Terminal::owned_ptr identifier_token;
    };
    
    class FileStatementUsing : public SyntaxNode, public PtrProtocol<FileStatementUsing> {
    public:
          FileStatementUsing(AccessModifier::owned_ptr _access_modifier,
                             Terminal::owned_ptr _using,
                             Terminal::owned_ptr _namespace,
                             Terminal::owned_ptr _namespace_name,
                             UsingAs::owned_ptr _using_as,
                             Terminal::owned_ptr _semicolon);
      ~FileStatementUsing();
    private:
      const AccessModifier & get_access_modifier() const;
      std::string & get_namespace() const;
      const UsingAs &get_using_as() const;
      
      AccessModifier::owned_ptr access_modifier;
      Terminal::owned_ptr using_token;
      Terminal::owned_ptr namespace_token;
      Terminal::owned_ptr namespace_name_token;
      UsingAs::owned_ptr using_as;
      Terminal::owned_ptr semicolon_token;
    };

    /**
     * This class represents a list of statements at the file-level.
     * These statements are things like global variable declarations,
     * function declarations, function definitions, and type definitions
     * that appear at the top-level of the translation unit or
     * possibly nested inside namespaces at the translation unit level.
     */
    class FileStatementList : public SyntaxNode, public PtrProtocol<FileStatementList> {
    public:
      FileStatementList();
      ~FileStatementList();
      const std::vector<FileStatement::owned_ptr> & get_statements() const;
      void add_statement(FileStatement::owned_ptr statement);
    private:
      std::vector<FileStatement::owned_ptr> statements;
    };
    
    /**
     * This class represents a translation unit, the top-level
     * result of parsing a source-file.  All syntax information
     * about the file appears in a tree-like structure beneath
     * this node.
     */
    class TranslationUnit : public SyntaxNode, public PtrProtocol<TranslationUnit> {
    public:
      
      TranslationUnit(
                      FileStatementList::owned_ptr file_statement_list,
                      Terminal::owned_ptr yyeof_token
                      );
      ~TranslationUnit();
      FileStatementList::raw_ptr get_statements();
    private:
      Terminal::owned_ptr yyeof_token;
      FileStatementList::owned_ptr file_statement_list;
    };
    
    
  };
  
};

