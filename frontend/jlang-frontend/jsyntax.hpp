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
  class FunctionDefinitionArgList;

  class ExpressionPrimary;
    
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
        FunctionDefinitionArgList*,

        ExpressionPrimary *,
      
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
      UnsafeModifier();
      ~UnsafeModifier();
    };

    class TypeSpecifier : public SyntaxNode, public PtrProtocol<TypeSpecifier> {
    public:
      TypeSpecifier();
      ~TypeSpecifier();
    };

    class FunctionDefinitionArgList : public SyntaxNode, public PtrProtocol<FunctionDefinitionArgList> {
    public:
      FunctionDefinitionArgList();
      ~FunctionDefinitionArgList();
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
      ~ArrayLength();
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
      FileStatementNamespace(NamespaceDeclaration::owned_ptr _namespace_declaration);
      ~FileStatementNamespace();
      const NamespaceDeclaration & get_declaration() const;
    private:
      NamespaceDeclaration::owned_ptr namespace_declaration;
    };
    
    class FileStatementUsing : public SyntaxNode, public PtrProtocol<FileStatementUsing> {
    public:
    private:
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

