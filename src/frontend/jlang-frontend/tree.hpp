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
      /**
       * This token is a single-line double-slash (C++-style)
       * comment.
       */
      EXTRA_COMMENT_SINGLE_LINE,
      /**
       * This token is a multi-line slash-star (C-style)
       * comment.
       */
      EXTRA_COMMENT_MULTI_LINE,
      /**
       * This token consists of whitespace between parsed
       * tokens.
       */
      EXTRA_WHITESPACE,
      /**
       * This token consists of a file metadata token
       * used in error reporting for generated files
       * so that errors can be traced back to their origin
       * source-files.
       */
      EXTRA_FILE_METADATA
    }  Type;

    /**
     * Constructs a non-syntax node corresponding to the
     * given lexical token of the given token type.
     */
    TerminalNonSyntax(Type _type, const Token & _token);
    /**
     * Destructor, nothing special.
     */
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
    
  private:
    Type type;
    const Token & token;
  };
  
  /**
   * Terminals are the raw tokens received by the lexer.
   * They carry the token information as well as any
   * "Non-syntax" data like comments and whitespace.
   */
  class Terminal : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Construct a terminal from the corresponding
     * lexer token.
     */
    Terminal(const Token & _token);
    /**
     * Destructor, nothing special.
     */
    ~Terminal();

    /**
     * Returns the type of the correspinding
     * lexer token.
     */
    const std::string & get_type() const;
    /**
     * Returns the matched data from the input
     * that matched the token.
     */
    const std::string & get_value() const;
    /**
     * Returns the source line number where the
     * input token was found.
     */
    const size_t get_line() const;
    /**
     * Returns the source column number where
     * the input token was found.
     */
    const size_t get_column() const;

    /**
     * For the case of identifier tokens, this returns
     * the fully-qualified name of the type, namespace,
     * or identifier once namespace resolution has
     * identified it.
     */
    const std::string & get_fully_qualified_name() const;
    /**
     * Used by the lexer once the fully-qualified token name
     * is resolved for identifier, type, and namespace tokens.
     */
    void set_fully_qualified_name(std::string name);
    
    // The terminal "owns" uniquely all of the non-syntax data
    // in this vector.  It may be returned to access it,
    // but these are owned pointers, so they must only
    // be de-referenced and never assigned to
    std::vector<::JLang::owned<TerminalNonSyntax>> non_syntax;
  private:
    const Token & token;
    std::string fully_qualified_name;
  };
  

  /**
   * A statement at the file level consists of one of the following types of
   * statement.
   * * Function Definition : Definition of a function along with the statements that make it up.
   * * Function Declaration : Declaration of a function (a.k.a. a prototype).
   * * Global Definition : Definition of a global variable.
   * * Class Definition : Definition of a composite type with associated methods.
   * * Enum Definition : Definition of an enumerated list of symbolic values.
   * * Type Definition : Definition (usually an alias) for a primitive or class type.
   * * Namespace : Definition of a namespace with other statements contained inside it.
   * * Using Statement : Adds a namespace to the search path when resolving types or namespaces.
   */
  class FileStatement : public JLang::frontend::ast::SyntaxNode {
  public:
    typedef std::variant<
            ::JLang::owned<FileStatementFunctionDefinition>,
            ::JLang::owned<FileStatementFunctionDeclaration>,
            ::JLang::owned<FileStatementGlobalDefinition>,
            ::JLang::owned<ClassDefinition>,
            ::JLang::owned<EnumDefinition>,
            ::JLang::owned<TypeDefinition>,
            ::JLang::owned<FileStatementNamespace>,
            ::JLang::owned<FileStatementUsing>> FileStatementType;
    
    FileStatement(FileStatementType _statement, const JLang::frontend::ast::SyntaxNode & _sn);
    ~FileStatement();
    
    /**
     * Returns the details of a specific statement.
     * The specific type of statement can be interrogated by
     * determining if the variant "holds" the given type of data
     * and then accessing the held statement.
     *
     * Example usage:
     * <pre>
     *   const auto & stmt = fs.get_statement();
     *   if (std::holds_alternative<FileStatementFunctionDefinition>(stmt)) {
     *       const auto & function_def = std::get<JLang::owned<FileStatementFunctionDefinition>>(stmt);
     *   }
     * </pre>
     */
    const FileStatementType & get_statement() const;
    
  private:
    FileStatementType statement;
  };
  
  /**
   * Indicates what type of access is permitted for the
   * value.  If it is marked as VOLATILE then read and write
   * will be forced from memory rather than allowing them
   * to be optimized out.  If it is marked as CONST then
   * the value is read-only in this context.  If it is
   * UNSPECIFIED then read/write access is un-restricted
   */
  class AccessQualifier : public JLang::frontend::ast::SyntaxNode {
  public:
    typedef enum {
      /**
       * If not specified, the access qualifier is unspecified
       * indicating that the variable has read/write access
       * and the optimizer is free to skip physical memory
       * access in order to resolve the value using registers
       * or other calculation techniques.
       */
      UNSPECIFIED,
      /**
       * Indicates that the value should be read and written
       * to memory as specified and that the optimizer must not
       * skip memory access.  This is mainly used when memory mapped
       * I/O is used in order to ensure that the physical memory
       * is written to so that devices on the memory bus may
       * observe and interact with the values.
       */
      VOLATILE,
      /**
       * Indicates that write access should not be permitted
       * in this context.  This is a way to communicate with
       * other programmers what values they may and may not
       * alter.
       */
      CONST
    } AccessQualifierType;
    /**
     * Constructs an access qualifier from a parse token
     * which will be either VOLATILE or CONST.
     */
    AccessQualifier(::JLang::owned<Terminal> _qualifier);
    /**
     * Constructs a default access qualifier (UNSPECIFIED)
     * if no qualifier is specified in the source file.
     */
    AccessQualifier();
    /**
     * Destructor, nothing special.
     */
    ~AccessQualifier();
    /**
     * Returns the access qualification type.
     */
    const AccessQualifier::AccessQualifierType & get_type() const;
  private:
    AccessQualifier::AccessQualifierType type;
    ::JLang::owned<Terminal> qualifier;
  };
  
  /**
   * An access modifier is a declaration of the visibility of
   * an object.  If it is not specified, it is presumed to
   * be public.  If it is specified, then the protection will
   * be evaluated by the namespace system according to where
   * it is referenced with respect to where it is declared.
   */
  class AccessModifier : public JLang::frontend::ast::SyntaxNode {
  public:
    typedef enum {
      /**
       * Indicates that this value can be accessed from anywhere.
       */
      PUBLIC,
      /**
       * Indicates that this value can be accessed from within
       * the context of the parent class or namespace.
       */
      PROTECTED,
      /**
       * Indicates that this value can only be accessed from
       * within the current class or namespace.
       */
      PRIVATE
    } AccessModifierType;
    /**
     * Constructs an access modifier for the given
     * terminal which will be either PUBLIC, PROTECTED, or PRIVATE.
     */
    AccessModifier(::JLang::owned<Terminal> _modifier);
    /**
     * This is the default constructor used when the
     * access modifier is not explicitly specified in the
     * source file (i.e. optional).
     */
    AccessModifier();
    /**
     * Destructor, nothing special.
     */
    ~AccessModifier();
    /**
     * Returns the type of modifier applied.
     */
    const AccessModifierType & get_type() const;
  private:
    AccessModifierType type;
    ::JLang::owned<Terminal> modifier;
  };

  /**
   * This is used to mark a function or a scope block as making
   * use of unsafe language features.  This is inspired by the Rust
   * keyword of the same name.  The semantics are such that
   * unsafe blocks may use unsafe language features such as
   * access to raw pointers, but the programmer is expected
   * to make sure that the access is safe in practice even if
   * the compiler cannot prove this.  Functions declared as unsafe
   * may also only be used inside unsafe blocks, so that the
   * unsafe code may be contained to only specifically marked
   * areas of the code.
   */
  class UnsafeModifier : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Constructs an unsafe modifier from the parse token
     * in the input.  This indicates that a block HAS been marked
     * as unsafe.
     */
    UnsafeModifier(::JLang::owned<Terminal> _unsafe_token);
    /**
     * The default unsafe modifier (i.e. not specified) refers
     * to a block that is NOT marked as unsafe.
     */
    UnsafeModifier();
    /**
     * Destructor, nothing special.
     */
    ~UnsafeModifier();
    /**
     * Returns true if the block or function has been marked
     * as unsafe.
     */
    bool is_unsafe() const;
  private:
    ::JLang::owned<Terminal> unsafe_token;
  };

  class TypeName : public JLang::frontend::ast::SyntaxNode {
  public:
    TypeName(::JLang::owned<Terminal> _type_name);
    TypeName(::JLang::owned<Terminal> _typeof_token,
             ::JLang::owned<Terminal> _paren_l_token,
             ::JLang::owned<Expression> _expression,
             ::JLang::owned<Terminal> _paren_r_token
             );
    ~TypeName();
    bool is_expression() const;
    const std::string & get_name() const;
    const Expression & get_expression() const;
  private:
    bool m_is_expression;
    // For raw names
    ::JLang::owned<Terminal> type_name;
    // for typeof expressions
    ::JLang::owned<Terminal> typeof_token;
    ::JLang::owned<Terminal> paren_l_token;
    ::JLang::owned<Expression> expression;
    ::JLang::owned<Terminal> paren_r_token;
  };
    
    class TypeSpecifierCallArgs : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierCallArgs();
      ~TypeSpecifierCallArgs();
      const std::vector<::JLang::owned<TypeSpecifier>> & get_arguments() const;
      void add_argument(::JLang::owned<TypeSpecifier> _argument);
      void add_argument(::JLang::owned<Terminal> _comma_token, ::JLang::owned<TypeSpecifier> _argument);
    private:
      std::vector<::JLang::owned<Terminal>> comma_list;
      std::vector<::JLang::owned<TypeSpecifier>> arguments;
    };

    class TypeSpecifierSimple : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierSimple(
                          ::JLang::owned<AccessQualifier> _access_qualifier,
                          ::JLang::owned<TypeName> _type_name
                          );
      ~TypeSpecifierSimple();
      const AccessQualifier & get_access_qualifier() const;
      const TypeName & get_type_name() const;
    private:
      ::JLang::owned<AccessQualifier> access_qualifier;
      ::JLang::owned<TypeName> type_name;
    };
    class TypeSpecifierTemplate : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierTemplate(
                            ::JLang::owned<TypeSpecifier> _type_specifier,
                            ::JLang::owned<Terminal> _paren_l_token,
                            ::JLang::owned<TypeSpecifierCallArgs> _type_specifier_call_args,
                            ::JLang::owned<Terminal> _paren_r_token
                            );
      ~TypeSpecifierTemplate();
      const TypeSpecifier & get_type() const;
      const TypeSpecifierCallArgs & get_args() const;
    private:
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<TypeSpecifierCallArgs> type_specifier_call_args;
      ::JLang::owned<Terminal> paren_r_token;
    };
    class TypeSpecifierFunctionPointer : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierFunctionPointer(
                                   ::JLang::owned<TypeSpecifier> _type_specifier,
                                   ::JLang::owned<Terminal> _paren_l1_token,
                                   ::JLang::owned<Terminal> _star_token,
                                   ::JLang::owned<Terminal> _identifier_token,
                                   ::JLang::owned<Terminal> _paren_r1_token,
                                   ::JLang::owned<Terminal> _paren_l2_token,
                                   ::JLang::owned<FunctionDefinitionArgList> _function_definition_arg_list,
                                   ::JLang::owned<Terminal> _paren_r2_token
                                   );
      ~TypeSpecifierFunctionPointer();
      const TypeSpecifier & get_return_type() const;
      const std::string & get_name() const;
      const FunctionDefinitionArgList & get_args() const;
    private:
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> paren_l1_token;
      ::JLang::owned<Terminal> star_token;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<Terminal> paren_r1_token;
      ::JLang::owned<Terminal> paren_l2_token;
      ::JLang::owned<FunctionDefinitionArgList> function_definition_arg_list;
      ::JLang::owned<Terminal> paren_r2_token;
    };
    class TypeSpecifierPointerTo : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierPointerTo(
                             ::JLang::owned<TypeSpecifier> _type_specifier,
                             ::JLang::owned<Terminal> _star_token,
                             ::JLang::owned<AccessQualifier> _access_qualifier
                             );
      ~TypeSpecifierPointerTo();
      const TypeSpecifier & get_type_specifier() const;
      const AccessQualifier & get_access_qualifier() const;
    private:
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> star_token;
      ::JLang::owned<AccessQualifier> access_qualifier;
    };
    class TypeSpecifierReferenceTo : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeSpecifierReferenceTo(
                               ::JLang::owned<TypeSpecifier> _type_specifier,
                               ::JLang::owned<Terminal> _andpersand_token,
                               ::JLang::owned<AccessQualifier> _access_qualifier
                               );
      ~TypeSpecifierReferenceTo();
      const TypeSpecifier & get_type_specifier() const;
      const AccessQualifier & get_access_qualifier() const;
    private:
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> andpersand_token;
      ::JLang::owned<AccessQualifier> access_qualifier;
    };
    
    class TypeSpecifier : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        ::JLang::owned<TypeSpecifierSimple>,
        ::JLang::owned<TypeSpecifierTemplate>,
        ::JLang::owned<TypeSpecifierFunctionPointer>,
        ::JLang::owned<TypeSpecifierPointerTo>,
        ::JLang::owned<TypeSpecifierReferenceTo>
      > TypeSpecifierType;
      TypeSpecifier(TypeSpecifier::TypeSpecifierType _type, const JLang::frontend::ast::SyntaxNode & _sn);
      ~TypeSpecifier();
      const TypeSpecifier::TypeSpecifierType & get_type() const;
    private:
      TypeSpecifier::TypeSpecifierType type;
    };

    class FunctionDefinitionArg : public JLang::frontend::ast::SyntaxNode {
    public:
      FunctionDefinitionArg(::JLang::owned<TypeSpecifier> _type_specifier,
                            ::JLang::owned<Terminal> _identifier_token
                            );
      ~FunctionDefinitionArg();
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
    private:
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> identifier_token;
    };
    class FunctionDefinitionArgList : public JLang::frontend::ast::SyntaxNode {
    public:
      FunctionDefinitionArgList();
      ~FunctionDefinitionArgList();
      const std::vector<::JLang::owned<FunctionDefinitionArg>> & get_arguments() const;
      void add_argument(::JLang::owned<FunctionDefinitionArg> _argument);
      void add_comma(::JLang::owned<Terminal> _comma);
    private:
      std::vector<::JLang::owned<Terminal>> commas;
      std::vector<::JLang::owned<FunctionDefinitionArg>> arguments;
    };

    class FileStatementFunctionDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementFunctionDeclaration(
                                      ::JLang::owned<AccessModifier> _access_modifier,
                                      ::JLang::owned<UnsafeModifier> _unsafe_modifier,
                                      ::JLang::owned<TypeSpecifier> _type_specifier,
                                      ::JLang::owned<Terminal> _name,
                                      ::JLang::owned<Terminal> _paren_l,
                                      ::JLang::owned<FunctionDefinitionArgList> _arguments,
                                      ::JLang::owned<Terminal> _paren_r,
                                      ::JLang::owned<Terminal> _semicolon
                                      );
      ~FileStatementFunctionDeclaration();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
      
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<UnsafeModifier> unsafe_modifier;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> name; // function name (IDENTIFIER)
      ::JLang::owned<Terminal> paren_l; // argument list delimiter PAREN_L
      ::JLang::owned<FunctionDefinitionArgList> arguments;
      ::JLang::owned<Terminal> paren_r; // argument list delimiter PAREN_R
      ::JLang::owned<Terminal> semicolon; // argument list delimiter SEMICOLON
    };


    class StatementVariableDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementVariableDeclaration(
                                   ::JLang::owned<TypeSpecifier> _type_specifier,
                                   ::JLang::owned<Terminal> _identifier_token,
                                   ::JLang::owned<ArrayLength> _array_length,
                                   ::JLang::owned<GlobalInitializer> _global_initializer,
                                   ::JLang::owned<Terminal> _semicolon_token
                                   );
      ~StatementVariableDeclaration();
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const ArrayLength & get_array_length() const;
      const GlobalInitializer & get_initializer() const;
    private:
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<ArrayLength> array_length;
      ::JLang::owned<GlobalInitializer> global_initializer;
      ::JLang::owned<Terminal> semicolon_token;
    private:
    };
    class StatementBlock : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementBlock(
                     ::JLang::owned<UnsafeModifier> _unsafe_modifier,
                     ::JLang::owned<ScopeBody> _scope_body
                     );
      ~StatementBlock();
      const UnsafeModifier & get_unsafe_modifier() const;
      const ScopeBody & get_scope_body() const;
    private:
      ::JLang::owned<UnsafeModifier> unsafe_modifier;
      ::JLang::owned<ScopeBody> scope_body;
    private:
    };
    class StatementExpression : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementExpression(
                          ::JLang::owned<Expression> _expression,
                          ::JLang::owned<Terminal> _semicolon_token
                          );
      ~StatementExpression();
      const Expression & get_expression() const;
    private:
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> semicolon_token;
    };
    class StatementIfElse;
    class StatementIfElse : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementIfElse(
                      ::JLang::owned<Terminal> _if_token,
                      ::JLang::owned<Terminal> _paren_l_token,
                      ::JLang::owned<Expression> _expression,
                      ::JLang::owned<Terminal> _paren_r_token,
                      ::JLang::owned<ScopeBody> _if_scope_body,
                      ::JLang::owned<Terminal> _else_token,
                      ::JLang::owned<ScopeBody> _else_scope_body
                      );
      StatementIfElse(
                      ::JLang::owned<Terminal> _if_token,
                      ::JLang::owned<Terminal> _paren_l_token,
                      ::JLang::owned<Expression> _expression,
                      ::JLang::owned<Terminal> _paren_r_token,
                      ::JLang::owned<ScopeBody> _if_scope_body,
                      ::JLang::owned<Terminal> _else_token,
                      ::JLang::owned<StatementIfElse> _statement_if_else
                      );
      StatementIfElse(
                      ::JLang::owned<Terminal> _if_token,
                      ::JLang::owned<Terminal> _paren_l_token,
                      ::JLang::owned<Expression> _expression,
                      ::JLang::owned<Terminal> _paren_r_token,
                      ::JLang::owned<ScopeBody> _if_scope_body
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
      ::JLang::owned<Terminal> if_token;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> paren_r_token;
      ::JLang::owned<ScopeBody> if_scope_body;
      ::JLang::owned<Terminal> else_token;
      ::JLang::owned<ScopeBody> else_scope_body;
      ::JLang::owned<StatementIfElse> else_if;
    };
    class StatementWhile : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementWhile(
                     ::JLang::owned<Terminal> _while_token,
                     ::JLang::owned<Terminal> _paren_l_token,
                     ::JLang::owned<Expression> _expression,
                     ::JLang::owned<Terminal> _paren_r_token,
                     ::JLang::owned<ScopeBody> _scope_body
                     );
     ~StatementWhile();
      const Expression & get_expression() const;
      const ScopeBody & get_scope_body() const;
    private:
      ::JLang::owned<Terminal> while_token;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> paren_r_token;
      ::JLang::owned<ScopeBody> scope_body;
    };
    class StatementFor : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementFor(
                   ::JLang::owned<Terminal> _for_token,
                   ::JLang::owned<Terminal> _paren_l_token,
                   ::JLang::owned<Expression> _expression_initial,
                   ::JLang::owned<Terminal> _semicolon_initial,
                   ::JLang::owned<Expression> _expression_termination,
                   ::JLang::owned<Terminal> _semicolon_termination,
                   ::JLang::owned<Expression> _expression_increment,
                   ::JLang::owned<Terminal> _paren_r_token,
                   ::JLang::owned<ScopeBody> _scope_body
                   );
      ~StatementFor();
      const Expression & get_expression_initial() const;
      const Expression & get_expression_termination() const;
      const Expression & get_expression_increment() const;
      const ScopeBody & get_scope_body() const;
    private:
      ::JLang::owned<Terminal> for_token;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<Expression> expression_initial;
      ::JLang::owned<Terminal> semicolon_initial;
      ::JLang::owned<Expression> expression_termination;
      ::JLang::owned<Terminal> semicolon_termination;
      ::JLang::owned<Expression> expression_increment;
      ::JLang::owned<Terminal> paren_r_token;
      ::JLang::owned<ScopeBody> scope_body;
    };

    class StatementSwitchBlock : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitchBlock(
                           ::JLang::owned<Terminal> _default_token,
                           ::JLang::owned<Terminal> _colon_token,
                           ::JLang::owned<ScopeBody> _scope_body
                           );
      StatementSwitchBlock(
                           ::JLang::owned<Terminal> _case_token,
                           ::JLang::owned<Expression> _expression,
                           ::JLang::owned<Terminal> _colon_token,
                           ::JLang::owned<ScopeBody> _scope_body
                           );
      ~StatementSwitchBlock();
      bool is_default() const;
      const Expression & get_expression();
      const ScopeBody & get_scope_body();
    private:
      bool m_is_default;
      // For the default case
      ::JLang::owned<Terminal> default_token;
      // For the expression case
      ::JLang::owned<Terminal> case_token;
      ::JLang::owned<Expression> expression;
      // Common:
      ::JLang::owned<Terminal> colon_token;
      ::JLang::owned<ScopeBody> scope_body;
    };
    
    class StatementSwitchContent : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitchContent();
      ~StatementSwitchContent();
      const std::vector<::JLang::owned<StatementSwitchBlock>> & get_blocks() const;
      void add_block(::JLang::owned<StatementSwitchBlock> _block);
    private:
      std::vector<::JLang::owned<StatementSwitchBlock>> blocks;
    };
    
    class StatementSwitch : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitch(
                      ::JLang::owned<Terminal> _switch_token,
                      ::JLang::owned<Terminal> _paren_l_token,
                      ::JLang::owned<Expression> expression,
                      ::JLang::owned<Terminal> _paren_r_token,
                      ::JLang::owned<Terminal> _brace_l_token,
                      ::JLang::owned<StatementSwitchContent> _switch_content,
                      ::JLang::owned<Terminal> _brace_r_token
                      );
      ~StatementSwitch();
      const Expression & get_expression() const;
      const StatementSwitchContent & get_switch_content() const;
    private:
      ::JLang::owned<Terminal> switch_token;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> paren_r_token;
      ::JLang::owned<Terminal> brace_l_token;
      ::JLang::owned<StatementSwitchContent> switch_content;
      ::JLang::owned<Terminal> brace_r_token;
    };
    class StatementLabel : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementLabel(
                     ::JLang::owned<Terminal> _label_token,
                     ::JLang::owned<Terminal> _identifier_token,
                     ::JLang::owned<Terminal> _colon_token
                     );
      ~StatementLabel();
      const std::string & get_name() const;
    private:
      ::JLang::owned<Terminal> label_token;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<Terminal> colon_token;
    };
    class StatementGoto : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementGoto(
                    ::JLang::owned<Terminal> _goto_token,
                    ::JLang::owned<Terminal> _identifier_token,
                    ::JLang::owned<Terminal> _semicolon_token
                    );
      ~StatementGoto();
      const std::string & get_label() const;
    private:
      ::JLang::owned<Terminal> goto_token;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<Terminal> semicolon_token;
    };
    class StatementBreak : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementBreak(
                     ::JLang::owned<Terminal> _break_token,
                     ::JLang::owned<Terminal> _semicolon_token
                     );
      ~StatementBreak();
    private:
      ::JLang::owned<Terminal> break_token;
      ::JLang::owned<Terminal> semicolon_token;
    };
    class StatementContinue : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementContinue(
                        ::JLang::owned<Terminal> _continue_token,
                        ::JLang::owned<Terminal> _semicolon_token
                        );
      ~StatementContinue();
    private:
      ::JLang::owned<Terminal> continue_token;
      ::JLang::owned<Terminal> semicolon_token;
    };
    class StatementReturn : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementReturn(
                      ::JLang::owned<Terminal> _return_token,
                      ::JLang::owned<Expression> _expression,
                      ::JLang::owned<Terminal> _semicolon_token
                      );
      ~StatementReturn();
      const Expression & get_expression() const;
    private:
      ::JLang::owned<Terminal> return_token;
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> semicolon_token;
    };

    class Statement : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
            ::JLang::owned<StatementVariableDeclaration>,
            ::JLang::owned<StatementBlock>,
            ::JLang::owned<StatementExpression>,
            ::JLang::owned<StatementIfElse>,
            ::JLang::owned<StatementWhile>,
            ::JLang::owned<StatementFor>,
            ::JLang::owned<StatementSwitch>,
            ::JLang::owned<StatementLabel>,
            ::JLang::owned<StatementGoto>,
            ::JLang::owned<StatementBreak>,
            ::JLang::owned<StatementContinue>,
            ::JLang::owned<StatementReturn>
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
      void add_statement(::JLang::owned<Statement> _statement);
      const std::vector<::JLang::owned<Statement>> &get_statements() const;
    private:
      std::vector<::JLang::owned<Statement>> statements;
    };
    
    class ScopeBody : public JLang::frontend::ast::SyntaxNode {
    public:
      ScopeBody(
                ::JLang::owned<Terminal> brace_l_token,
                ::JLang::owned<StatementList> statement_list,
                ::JLang::owned<Terminal> brace_r_token
                );
      ~ScopeBody();
      const StatementList & get_statements() const;
    private:
      ::JLang::owned<Terminal> brace_l_token;
      ::JLang::owned<StatementList> statement_list;
      ::JLang::owned<Terminal> brace_r_token;
    };
    
    class FileStatementFunctionDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementFunctionDefinition(
                                      ::JLang::owned<AccessModifier> _access_modifier,
                                      ::JLang::owned<UnsafeModifier> _unsafe_modifier,
                                      ::JLang::owned<TypeSpecifier> _type_specifier,
                                      ::JLang::owned<Terminal> _name,
                                      ::JLang::owned<Terminal> _paren_l,
                                      ::JLang::owned<FunctionDefinitionArgList> _arguments,
                                      ::JLang::owned<Terminal> _paren_r,
                                      ::JLang::owned<ScopeBody> _scope_body
                                      );
      ~FileStatementFunctionDefinition();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
      const ScopeBody & get_scope_body() const;
      
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<UnsafeModifier> unsafe_modifier;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> name; // function name (IDENTIFIER)
      ::JLang::owned<Terminal> paren_l; // argument list delimiter PAREN_L
      ::JLang::owned<FunctionDefinitionArgList> arguments;
      ::JLang::owned<Terminal> paren_r; // argument list delimiter PAREN_R
      ::JLang::owned<ScopeBody> scope_body; // argument list delimiter SEMICOLON
    };

    class ArrayLength : public JLang::frontend::ast::SyntaxNode {
    public:
      ArrayLength();
      ArrayLength(
                  ::JLang::owned<Terminal> _bracket_l_token,
                  ::JLang::owned<Terminal> _literal_int_token,
                  ::JLang::owned<Terminal> _bracket_r_token
                  );
      ~ArrayLength();
      bool is_array() const;
      size_t get_size() const;
    private:
      ::JLang::owned<Terminal> bracket_l_token;
      ::JLang::owned<Terminal> literal_int_token;
      ::JLang::owned<Terminal> bracket_r_token;
    };

    class ClassDeclStart : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassDeclStart(
                     ::JLang::owned<AccessModifier> _access_modifier,
                     ::JLang::owned<Terminal> _class_token,
                     ::JLang::owned<Terminal> _identifier_token,
                     ::JLang::owned<ClassArgumentList> _class_argument_list
                     );
                     
      ~ClassDeclStart();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const ClassArgumentList & get_argument_list() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<Terminal> class_token;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<ClassArgumentList> class_argument_list;
    };
    class ClassArgumentList : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassArgumentList();
      ClassArgumentList(::JLang::owned<Terminal> _argument);
      ~ClassArgumentList();
      void add_argument(::JLang::owned<Terminal> _comma, ::JLang::owned<Terminal> _argument);
      void add_parens(::JLang::owned<Terminal> _paren_l, ::JLang::owned<Terminal> _paren_r);
      const std::vector<::JLang::owned<Terminal>> & get_arguments() const;
    private:
      ::JLang::owned<Terminal> paren_l;
      std::vector<::JLang::owned<Terminal>> comma_list;
      std::vector<::JLang::owned<Terminal>> argument_list;
      ::JLang::owned<Terminal> paren_r;
    };

    class ClassMemberDeclarationVariable : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationVariable(
                                     ::JLang::owned<AccessModifier> _access_modifier,
                                     ::JLang::owned<TypeSpecifier> _type_specifier,
                                     ::JLang::owned<Terminal> _identifier_token,
                                     ::JLang::owned<ArrayLength> _array_length,
                                     ::JLang::owned<Terminal> _semicolon_token
                                     );
      ~ClassMemberDeclarationVariable();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const ArrayLength & get_array_length() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<ArrayLength> array_length;
      ::JLang::owned<Terminal> semicolon_token;
    };    
    class ClassMemberDeclarationMethod : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationMethod(
                                     ::JLang::owned<AccessModifier> _access_modifier,
                                     ::JLang::owned<TypeSpecifier> _type_specifier,
                                     ::JLang::owned<Terminal> _identifier_token,
                                     ::JLang::owned<Terminal> _paren_l_token,
                                     ::JLang::owned<FunctionDefinitionArgList> _function_definition_arg_list,
                                     ::JLang::owned<Terminal> _paren_r_token,
                                     ::JLang::owned<Terminal> _semicolon_token
                                   );
      ~ClassMemberDeclarationMethod();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<FunctionDefinitionArgList> function_definition_arg_list;
      ::JLang::owned<Terminal> paren_r_token;
      ::JLang::owned<Terminal> semicolon_token;
    };
    class ClassMemberDeclarationConstructor : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationConstructor(
                                        ::JLang::owned<AccessModifier> _access_modifier,
                                        ::JLang::owned<TypeSpecifier> _type_specifier,
                                        ::JLang::owned<Terminal> _paren_l_token,
                                        ::JLang::owned<FunctionDefinitionArgList> _function_definition_arg_list,
                                        ::JLang::owned<Terminal> _paren_r_token,
                                        ::JLang::owned<Terminal> _semicolon_token
                                        );
      ~ClassMemberDeclarationConstructor();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<FunctionDefinitionArgList> function_definition_arg_list;
      ::JLang::owned<Terminal> paren_r_token;
      ::JLang::owned<Terminal> semicolon_token;
    };
    class ClassMemberDeclarationDestructor : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationDestructor(
                                       ::JLang::owned<AccessModifier> _access_modifier,
                                       ::JLang::owned<Terminal> _tilde_token,
                                       ::JLang::owned<TypeSpecifier> _type_specifier,
                                       ::JLang::owned<Terminal> _paren_l_token,
                                       ::JLang::owned<FunctionDefinitionArgList> _function_definition_arg_list,
                                       ::JLang::owned<Terminal> _paren_r_token,
                                       ::JLang::owned<Terminal> _semicolon_token
                                   );
      ~ClassMemberDeclarationDestructor();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<Terminal> tilde_token;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<FunctionDefinitionArgList> function_definition_arg_list;
      ::JLang::owned<Terminal> paren_r_token;
      ::JLang::owned<Terminal> semicolon_token;
    };
    
    class ClassMemberDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        ::JLang::owned<ClassMemberDeclarationVariable>,
        ::JLang::owned<ClassMemberDeclarationMethod>,
        ::JLang::owned<ClassMemberDeclarationConstructor>,
        ::JLang::owned<ClassMemberDeclarationDestructor>,
        ::JLang::owned<ClassDefinition>,
        ::JLang::owned<EnumDefinition>,
        ::JLang::owned<TypeDefinition>
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
      const std::vector<::JLang::owned<ClassMemberDeclaration>> & get_members() const;
      void add_member(::JLang::owned<ClassMemberDeclaration> _member);
    private:
      std::vector<::JLang::owned<ClassMemberDeclaration>> members;
    };
          
    class ClassDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassDefinition(
                      ::JLang::owned<ClassDeclStart> _class_decl_start,
                      ::JLang::owned<Terminal> _brace_l_token,
                      ::JLang::owned<ClassMemberDeclarationList> _class_member_declaration_list,
                      ::JLang::owned<Terminal> _brace_r_token,
                      ::JLang::owned<Terminal> _semicolon_token
                      );
      ~ClassDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const ClassArgumentList & get_argument_list() const;
      const ClassMemberDeclarationList & get_members() const;
    private:
      ::JLang::owned<ClassDeclStart> class_decl_start;
      ::JLang::owned<Terminal> brace_l_token;
      ::JLang::owned<ClassMemberDeclarationList> class_member_declaration_list;
      ::JLang::owned<Terminal> brace_r_token;
      ::JLang::owned<Terminal> semicolon_token;
    };

    class TypeDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeDefinition(
                     ::JLang::owned<AccessModifier> _access_modifier,
                     ::JLang::owned<Terminal> _typedef_token,
                     ::JLang::owned<TypeSpecifier> _type_specifier,
                     ::JLang::owned<Terminal> _identifier_token,
                     ::JLang::owned<Terminal> _semicolon_token
                     );
      ~TypeDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const TypeSpecifier & get_type_specifier() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<Terminal> typedef_token;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<Terminal> semicolon_token;
    };

    class EnumDefinitionValue : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinitionValue(
                          ::JLang::owned<Terminal> _identifier_token,
                          ::JLang::owned<Terminal> _equals_token,
                          ::JLang::owned<ExpressionPrimary> _expression_primary,
                          ::JLang::owned<Terminal> _semicolon_token
                          );                          
      ~EnumDefinitionValue();
      const std::string & get_name() const;
      const ExpressionPrimary & get_expression() const;
    private:
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<Terminal> equals_token;
      ::JLang::owned<ExpressionPrimary> expression_primary;
      ::JLang::owned<Terminal> semicolon_token;      
    };
    
    class EnumDefinitionValueList : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinitionValueList();
      ~EnumDefinitionValueList();
      void add_value(::JLang::owned<EnumDefinitionValue>);
      const std::vector<::JLang::owned<EnumDefinitionValue>> &get_values() const;
    private:
      std::vector<::JLang::owned<EnumDefinitionValue>> values;
    };

    class EnumDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinition(
                     ::JLang::owned<AccessModifier> _access_modifier,
                     ::JLang::owned<Terminal> _enum_token,
                     ::JLang::owned<Terminal> _type_name_token,
                     ::JLang::owned<Terminal> _identifier_token,
                     ::JLang::owned<Terminal> _brace_l_token,
                     ::JLang::owned<EnumDefinitionValueList> _enum_value_list,
                     ::JLang::owned<Terminal> _brace_r_token,
                     ::JLang::owned<Terminal> _semicolon_token
                     );
      ~EnumDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & type_name() const;
      const std::string & enum_name() const;
      const EnumDefinitionValueList & get_value_list() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<Terminal> enum_token;
      ::JLang::owned<Terminal> type_name_token;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<Terminal> brace_l_token;
      ::JLang::owned<EnumDefinitionValueList> enum_value_list;
      ::JLang::owned<Terminal> brace_r_token;
      ::JLang::owned<Terminal> semicolon_token;
      
    };

    class ExpressionPrimaryIdentifier : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryIdentifier(::JLang::owned<Terminal> _identifier_token);
      ~ExpressionPrimaryIdentifier();
      const std::string & get_identifier() const;
    private:
      ::JLang::owned<Terminal> identifier_token;
    };
    class ExpressionPrimaryNested : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryNested(
                              ::JLang::owned<Terminal> _paren_l_token,
                              ::JLang::owned<Expression> _expression,
                              ::JLang::owned<Terminal> _paren_r_token
                              );
      ~ExpressionPrimaryNested();
      const Expression & get_expression() const;
    private:
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> paren_r_token;
    };

    class ExpressionPrimaryLiteralInt : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralInt(
                                  ::JLang::owned<Terminal> literal_token
                                  );
      ~ExpressionPrimaryLiteralInt();
      const std::string & get_value() const;
    private:
      ::JLang::owned<Terminal> literal_token;
    };
    class ExpressionPrimaryLiteralChar : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralChar(
                                  ::JLang::owned<Terminal> _literal_token
                                  );
      ~ExpressionPrimaryLiteralChar();
      const std::string & get_value() const;
    private:
      ::JLang::owned<Terminal> literal_token;
    };
    class ExpressionPrimaryLiteralString : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralString(
                                     ::JLang::owned<Terminal> _literal_token
                                     );
      ~ExpressionPrimaryLiteralString();
      const std::string & get_value() const;
    private:
      ::JLang::owned<Terminal> literal_token;
    };
    class ExpressionPrimaryLiteralFloat : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralFloat(
                                  ::JLang::owned<Terminal> _literal_token
                                  );
      ~ExpressionPrimaryLiteralFloat();
      const std::string & get_value() const;
    private:
      ::JLang::owned<Terminal> literal_token;
    };
    
    class ExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<::JLang::owned<ExpressionPrimaryIdentifier>,
                           ::JLang::owned<ExpressionPrimaryNested>,
                           ::JLang::owned<ExpressionPrimaryLiteralChar>,
                           ::JLang::owned<ExpressionPrimaryLiteralString>,
                           ::JLang::owned<ExpressionPrimaryLiteralInt>,
                           ::JLang::owned<ExpressionPrimaryLiteralFloat>
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
                                  ::JLang::owned<Expression> _array_expression,
                                  ::JLang::owned<Terminal> _bracket_l_token,
                                  ::JLang::owned<Expression> _index_expression,
                                  ::JLang::owned<Terminal> _bracket_r_token
                                  );
      ~ExpressionPostfixArrayIndex();
      const Expression & get_array() const;
      const Expression & get_index() const;
    private:
      ::JLang::owned<Expression> array_expression;
      ::JLang::owned<Terminal> bracket_l_token;
      ::JLang::owned<Expression> index_expression;
      ::JLang::owned<Terminal> bracket_r_token;
    };

    class ArgumentExpressionList : public JLang::frontend::ast::SyntaxNode {
    public:
      ArgumentExpressionList();
      ~ArgumentExpressionList();
      const std::vector<::JLang::owned<Expression>> & get_arguments() const;
      void add_argument(::JLang::owned<Expression> _argument);
      void add_argument(::JLang::owned<Terminal> _comma_token, ::JLang::owned<Expression> _argument);
    private:
      std::vector<::JLang::owned<Terminal>> comma_list;
      std::vector<::JLang::owned<Expression>> arguments;
    };
    
    class ExpressionPostfixFunctionCall : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixFunctionCall(
                                    ::JLang::owned<Expression> _function_expression,
                                    ::JLang::owned<Terminal> _paren_l_token,
                                    ::JLang::owned<ArgumentExpressionList> _arguments,
                                    ::JLang::owned<Terminal> _paren_r_token
                                    );
      ~ExpressionPostfixFunctionCall();
      const Expression & get_function() const;
      const ArgumentExpressionList & get_arguments() const;
    private:
      ::JLang::owned<Expression> function_expression;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<ArgumentExpressionList> arguments;
      ::JLang::owned<Terminal> paren_r_token;
    };

    class ExpressionPostfixDot : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixDot(
                           ::JLang::owned<Expression> _expression,
                           ::JLang::owned<Terminal> _dot_token,
                           ::JLang::owned<Terminal> _identifier_token
                           );
      ~ExpressionPostfixDot();
      const Expression & get_expression() const;
      const std::string & get_identifier() const;
    private:
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> dot_token;
      ::JLang::owned<Terminal> identifier_token;
    };
    
    class ExpressionPostfixArrow : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixArrow(
                             ::JLang::owned<Expression> _expression,
                             ::JLang::owned<Terminal> _arrow_token,
                             ::JLang::owned<Terminal> _identifier_token
                             );
      ~ExpressionPostfixArrow();
      const Expression & get_expression() const;
      const std::string & get_identifier() const;
    private:
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> arrow_token;
      ::JLang::owned<Terminal> identifier_token;
    };

    class ExpressionPostfixIncDec : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef enum {
        INCREMENT,
        DECREMENT
      } OperationType;
      ExpressionPostfixIncDec(
                              ::JLang::owned<Expression> _expression,
                              ::JLang::owned<Terminal> _operator_token
                              );
      ~ExpressionPostfixIncDec();
      const ExpressionPostfixIncDec::OperationType & get_type();
      const Expression & get_expression();
    private:
      ExpressionPostfixIncDec::OperationType type;
      ::JLang::owned<Terminal> operator_token;
      ::JLang::owned<Expression> expression;
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
                            ::JLang::owned<Terminal> _operator_token,
                            ::JLang::owned<Expression> _expression
                            );
      ~ExpressionUnaryPrefix();
      const ExpressionUnaryPrefix::OperationType & get_type();
      const Expression & get_expression();
    private:
      ExpressionUnaryPrefix::OperationType type;
      ::JLang::owned<Terminal> operator_token;
      ::JLang::owned<Expression> expression;
    };

    class ExpressionUnarySizeofType : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionUnarySizeofType(
                                ::JLang::owned<Terminal> _sizeof_token,
                                ::JLang::owned<Terminal> _paren_l_token,
                                ::JLang::owned<TypeSpecifier> _type_specifier,
                                ::JLang::owned<Terminal> _paren_r_token
                                );
      ~ExpressionUnarySizeofType();
      const TypeSpecifier & get_type_specifier() const;
    private:
      ::JLang::owned<Terminal> sizeof_token;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> paren_r_token;
    };

    class ExpressionCast : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionCast(
                     ::JLang::owned<Terminal> _cast_token,
                     ::JLang::owned<Terminal> _paren_l_token,
                     ::JLang::owned<TypeSpecifier> _type_specifier,
                     ::JLang::owned<Terminal> _comma_token,
                     ::JLang::owned<Expression> _expression,
                     ::JLang::owned<Terminal> _paren_r_token
                     );
      ~ExpressionCast();
      const TypeSpecifier & get_type() const;
      const Expression & get_expression() const;
    private:
      ::JLang::owned<Terminal> cast_token;
      ::JLang::owned<Terminal> paren_l_token;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> comma_token;
      ::JLang::owned<Expression> expression;
      ::JLang::owned<Terminal> paren_r_token;
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
                       ::JLang::owned<Expression> _expression_a,
                       ::JLang::owned<Terminal> _operator_token,
                       ::JLang::owned<Expression> _expression_b
                       );
      ~ExpressionBinary();
      const Expression & get_a() const;
      const ExpressionBinary::OperationType & get_operator() const;
      const Expression & get_b() const;
      
    private:
      OperationType type;
      ::JLang::owned<Expression> expression_a;
      ::JLang::owned<Terminal> operator_token;
      ::JLang::owned<Expression> expression_b;
    };
    
    class ExpressionTrinary : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionTrinary(
                        ::JLang::owned<Expression> _condition,
                        ::JLang::owned<Terminal> _questionmark_token,
                        ::JLang::owned<Expression> _if_expression,
                        ::JLang::owned<Terminal> _colon_token,
                        ::JLang::owned<Expression> _else_expression
                        );
      ~ExpressionTrinary();
      const Expression & get_condition() const;
      const Expression & get_if() const;
      const Expression & get_else() const;
    private:
      ::JLang::owned<Expression> condition;
      ::JLang::owned<Terminal> questionmark_token;
      ::JLang::owned<Expression> if_expression;
      ::JLang::owned<Terminal> colon_token;
      ::JLang::owned<Expression> else_expression;      
    };
    
    class Expression : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<::JLang::owned<ExpressionPrimary>,
                           ::JLang::owned<ExpressionPostfixArrayIndex>,
                           ::JLang::owned<ExpressionPostfixFunctionCall>,
                           ::JLang::owned<ExpressionPostfixDot>,
                           ::JLang::owned<ExpressionPostfixArrow>,
                           ::JLang::owned<ExpressionPostfixIncDec>,
                           ::JLang::owned<ExpressionUnaryPrefix>,
                           ::JLang::owned<ExpressionUnarySizeofType>,
                           ::JLang::owned<ExpressionBinary>,
                           ::JLang::owned<ExpressionTrinary>,
                           ::JLang::owned<ExpressionCast>
                           > ExpressionType;
      Expression(Expression::ExpressionType _expression_type, const SyntaxNode & _sn);
      ~Expression();
      const Expression::ExpressionType & get_expression() const;
    private:
      Expression::ExpressionType expression_type;
    };
    
    class GlobalInitializerExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerExpressionPrimary(::JLang::owned<Terminal> _equals_token,
                                         ::JLang::owned<ExpressionPrimary> _expression
                                         );
      ~GlobalInitializerExpressionPrimary();
      const ExpressionPrimary & get_expression() const;
    private:
      ::JLang::owned<Terminal> equals_token;
      ::JLang::owned<ExpressionPrimary> expression;

    };
    class GlobalInitializerAddressofExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerAddressofExpressionPrimary(
                                                  ::JLang::owned<Terminal> _equals_token,
                                                  ::JLang::owned<Terminal> _addressof_token,
                                                  ::JLang::owned<ExpressionPrimary> _expression
                                         );
      ~GlobalInitializerAddressofExpressionPrimary();
      const ExpressionPrimary & get_expression() const;
    private:
      ::JLang::owned<Terminal> equals_token;
      ::JLang::owned<Terminal> addressof_token;
      ::JLang::owned<ExpressionPrimary> expression;
    };

    class StructInitializer : public JLang::frontend::ast::SyntaxNode {
    public:
      StructInitializer(
                        ::JLang::owned<Terminal> _dot_token,
                        ::JLang::owned<Terminal> _identifier_token,
                        ::JLang::owned<GlobalInitializer> _global_initializer,
                        ::JLang::owned<Terminal> _semicolon_token
                        );
      ~StructInitializer();
      const GlobalInitializer & get_initializer() const;
    private:
      ::JLang::owned<Terminal> dot_token;
      ::JLang::owned<Terminal> identifier_token;
      ::JLang::owned<GlobalInitializer> global_initializer;
      ::JLang::owned<Terminal> semicolon_token;
    };
          
    class StructInitializerList : public JLang::frontend::ast::SyntaxNode {
    public:
      StructInitializerList();
      ~StructInitializerList();
      void add_initializer(::JLang::owned<StructInitializer> initializer);
      const std::vector<::JLang::owned<StructInitializer>> & get_initializers() const;
    private:
      std::vector<::JLang::owned<StructInitializer>> initializers;
    };
          
    class GlobalInitializerStructInitializerList : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerStructInitializerList(
                                             ::JLang::owned<Terminal> _equals_token,
                                             ::JLang::owned<Terminal> _brace_l_token,
                                             ::JLang::owned<StructInitializerList> _struct_initializer,
                                             ::JLang::owned<Terminal> _brace_r_token
                                             );

      ~GlobalInitializerStructInitializerList();
      const StructInitializerList & get_struct_initializer() const;
    private:
      ::JLang::owned<Terminal> equals_token;
      ::JLang::owned<Terminal> brace_l_token;
      ::JLang::owned<StructInitializerList> struct_initializer;
      ::JLang::owned<Terminal> brace_r_token;
    };
    
    class GlobalInitializer : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        ::JLang::owned<GlobalInitializerExpressionPrimary>,
        ::JLang::owned<GlobalInitializerAddressofExpressionPrimary>,
        ::JLang::owned<GlobalInitializerStructInitializerList>,
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
                                    ::JLang::owned<AccessModifier> _access_modifier,
                                    ::JLang::owned<UnsafeModifier> _unsafe_modifier,
                                    ::JLang::owned<TypeSpecifier> _type_specifier,
                                    ::JLang::owned<Terminal> _name,
                                    ::JLang::owned<ArrayLength> _array_length,
                                    ::JLang::owned<GlobalInitializer> _global_initializer,
                                    ::JLang::owned<Terminal> _semicolon
                                    );
      ~FileStatementGlobalDefinition();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const ArrayLength & get_array_length() const;
      const GlobalInitializer & get_global_initializer() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<UnsafeModifier> unsafe_modifier;
      ::JLang::owned<TypeSpecifier> type_specifier;
      ::JLang::owned<Terminal> name; // function name (IDENTIFIER)
      ::JLang::owned<ArrayLength> array_length;
      ::JLang::owned<GlobalInitializer> global_initializer;
      ::JLang::owned<Terminal> semicolon;
    };
    
    class NamespaceDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      NamespaceDeclaration(
                           ::JLang::owned<AccessModifier> _access_modifier,
                           ::JLang::owned<Terminal> _namespace_token,
                           ::JLang::owned<Terminal> _identifier_token
                           );
      ~NamespaceDeclaration();
      const AccessModifier & get_access_modifier() const;
      const Terminal & get_name() const;
    private:
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<Terminal> namespace_token;
      ::JLang::owned<Terminal> identifier_token;
    };

    class FileStatementNamespace : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementNamespace(::JLang::owned<NamespaceDeclaration> _namespace_declaration,
                             ::JLang::owned<Terminal> _brace_l_token,
                             ::JLang::owned<FileStatementList> _file_statement_list,
                             ::JLang::owned<Terminal> _brace_r_token,
                             ::JLang::owned<Terminal> _semicolon_token
                             );
      ~FileStatementNamespace();
      const NamespaceDeclaration & get_declaration() const;
      const FileStatementList & get_statement_list() const;
    private:
      ::JLang::owned<NamespaceDeclaration> namespace_declaration;
      ::JLang::owned<Terminal> brace_l_token;
      ::JLang::owned<FileStatementList> file_statement_list;
      ::JLang::owned<Terminal> brace_r_token;
      ::JLang::owned<Terminal> semicolon_token;
    };

    class UsingAs : public JLang::frontend::ast::SyntaxNode {
    public:
      UsingAs(
              ::JLang::owned<Terminal> _as_token,
              ::JLang::owned<Terminal> _identifier_token
              );
      UsingAs();
      ~UsingAs();
      const std::string & get_using_name() const;
    private:
      std::string using_name;
      ::JLang::owned<Terminal> as_token;
      ::JLang::owned<Terminal> identifier_token;
    };
    
    class FileStatementUsing : public JLang::frontend::ast::SyntaxNode {
    public:
          FileStatementUsing(::JLang::owned<AccessModifier> _access_modifier,
                             ::JLang::owned<Terminal> _using,
                             ::JLang::owned<Terminal> _namespace,
                             ::JLang::owned<Terminal> _namespace_name,
                             ::JLang::owned<UsingAs> _using_as,
                             ::JLang::owned<Terminal> _semicolon);
      ~FileStatementUsing();
    private:
      const AccessModifier & get_access_modifier() const;
      const std::string & get_namespace() const;
      const UsingAs &get_using_as() const;
      
      ::JLang::owned<AccessModifier> access_modifier;
      ::JLang::owned<Terminal> using_token;
      ::JLang::owned<Terminal> namespace_token;
      ::JLang::owned<Terminal> namespace_name_token;
      ::JLang::owned<UsingAs> using_as;
      ::JLang::owned<Terminal> semicolon_token;
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
      FileStatementList(::JLang::owned<Terminal> _yyeof);
      ~FileStatementList();
      const std::vector<::JLang::owned<FileStatement>> & get_statements() const;
      void add_statement(::JLang::owned<FileStatement> statement);
    private:
      ::JLang::owned<Terminal> yyeof;
      std::vector<::JLang::owned<FileStatement>> statements;
    };
    
    /**
     * This class represents a translation unit, the top-level
     * result of parsing a source-file.  A translation unit
     * normally corresponds to a unit that will ultimately
     * be compiled into a binary object file (.o) and linked
     * later into an executable.
     *
     * A translation unit consists of zero or more file-level
     * statements that represent the file's content.  At this
     * level, the statements may be one of:
     * * Type Definition
     * * Enum defintion
     * * Class definition
     * * Function definition
     * * Global variable definition
     * * Function forward declaration;
     * * Namespace definition (possibly containing more statements inside them).
     * * Using (as) declaration.
     * 
     * All syntax information
     * about the file appears in a tree-like structure beneath
     * this node.
     */
    class TranslationUnit : public JLang::frontend::ast::SyntaxNode {
    public:
      TranslationUnit(
                      ::JLang::owned<FileStatementList> file_statement_list,
                      ::JLang::owned<Terminal> yyeof_token
                      );
      ~TranslationUnit();
      /**
       * This method returns a list of 'const' owned pointers to the
       * statements defined in this translation unit.
       */
      const std::vector<::JLang::owned<FileStatement>> & get_statements() const;
    private:
      ::JLang::owned<Terminal> yyeof_token;
      ::JLang::owned<FileStatementList> file_statement_list;
      
    };
    
};

/*! @} End of Doxygen Groups*/
