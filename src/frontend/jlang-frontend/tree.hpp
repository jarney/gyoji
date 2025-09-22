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
  //! Represents whitespace, comments, and metadata for the source file.
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
    TerminalNonSyntax(Type _type, const JLang::context::Token & _token);
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
    const JLang::context::Token & token;
  };

  //! Represents tokens from the lexer used to represent keywords and identifiers found in the source.
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
    Terminal(const JLang::context::Token & _token);
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
     * Returns a reference to the place in the source-file
     * where this terminal occurred.
     */
    const JLang::context::SourceReference & get_terminal_source_ref() const;
    
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
    std::vector<JLang::owned<TerminalNonSyntax>> non_syntax;
  private:
    const JLang::context::Token & token;
    std::string fully_qualified_name;
  };
  

  /**
   * A statement at the file level consists of one of the following types of
   * statement.
   * * Function Definition : Definition of a function along with the statements that make it up.
   * * Function Declaration : Declaration of a function (a.k.a. a prototype).
   * * Global Definition : Definition of a global variable.
   * * Class Declaration : Forward declaration of a class with an incomplete type.
   * * Class Definition : Definition of a composite type with associated methods.
   * * Enum Definition : Definition of an enumerated list of symbolic values.
   * * Type Definition : Definition (usually an alias) for a primitive or class type.
   * * Namespace : Definition of a namespace with other statements contained inside it.
   * * Using Statement : Adds a namespace to the search path when resolving types or namespaces.
   */
  class FileStatement : public JLang::frontend::ast::SyntaxNode {
  public:
    typedef std::variant<
            JLang::owned<FileStatementFunctionDefinition>,
            JLang::owned<FileStatementFunctionDeclaration>,
            JLang::owned<FileStatementGlobalDefinition>,
            JLang::owned<ClassDeclaration>,
            JLang::owned<ClassDefinition>,
            JLang::owned<EnumDefinition>,
            JLang::owned<TypeDefinition>,
            JLang::owned<FileStatementNamespace>,
            JLang::owned<FileStatementUsing>> FileStatementType;
    
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

  //! This indicates the type of access that may be permitted as UNSPECIFIED, CONST, or VOLATILE.
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
    AccessQualifier(JLang::owned<Terminal> _qualifier);
    /**
     * Constructs a default access qualifier (UNSPECIFIED)
     * if no qualifier is specified in the source file.
     */
    AccessQualifier(const JLang::context::SourceReference & _source_ref);
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
    JLang::owned<Terminal> qualifier;
  };

  //! Declares PUBLIC, PROTECTED, or PRIVATE access to functions and members.
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
    AccessModifier(JLang::owned<Terminal> _modifier);
    /**
     * This is the default constructor used when the
     * access modifier is not explicitly specified in the
     * source file (i.e. optional).
     */
    AccessModifier(const JLang::context::SourceReference & _source_ref);
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
    JLang::owned<Terminal> modifier;
  };

  //! Represents the safety/borrow check semantics for a function or block.
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
    UnsafeModifier(JLang::owned<Terminal> _unsafe_token);
    /**
     * The default unsafe modifier (i.e. not specified) refers
     * to a block that is NOT marked as unsafe.
     */
    UnsafeModifier(const JLang::context::SourceReference & _source_ref);
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
    JLang::owned<Terminal> unsafe_token;
  };

  //! Represents the name of a type.
  /**
   * This class represents a type which may be one of
   * several types.
   * * Terminal : A simple type type name such as "u8" or "char".
   *              The type named here may refer to a built-in primitive
   *              type or may refer to a composite type such as an enum
   *              or class.
   * * Expression : A type consisting of the "typeof" an expression,
   *                e.g. the type returned by an expression, assuming
   *                that the expression makes sense in that context.
   *                Note that the expression is not actually evaluated,
   *                but the type information is extracted from the
   *                expression and is used to express the type of
   *                the expression that would be yielded if that
   *                expression were evaluated.
   *  In the following example, a simple type of "float" would be returned:
   *  <pre>
   *    float f;
   *  </pre>
   *  In the next example, the type would ultimately resolve to "u32"
   *  because that is the result that would be yielded for this expression.
   *  <pre>
   *    u32 a;
   *    u32 b;
   *    typeof(a+b) x;
   *  </pre>
   * 
   */
  class TypeName : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Constructs a simple type name representing the
     * type specified in the source file.
     */
    TypeName(JLang::owned<Terminal> _type_name);

    /**
     * Constructs a 'typeof' expression where the type
     * has the meaning of the type of the expression
     * specified.
     */
    TypeName(JLang::owned<Terminal> _typeof_token,
             JLang::owned<Terminal> _paren_l_token,
             JLang::owned<Expression> _expression,
             JLang::owned<Terminal> _paren_r_token
             );
    /**
     * Destructor, nothing special.
     */
    ~TypeName();
    /**
     * Returns true if type is an expression type.
     * If this returns true, then it is safe to
     * call the "get_expression()" method,
     * otherwise the "get_name()" method must
     * be called instead.
     *
     * For example, the usage is expected to
     * be equivalent to the following.
     * <pre>
     *     if (type_name.is_expression()) {
     *         const Expression expr = type_name.get_expression();
     *     }
     *     else {
     *         const std::string & name = type_name.get_name();
     *     }
     * </pre>
     * Failure to check the type before de-referencing
     * will result in a segmentation violation.
     */
    bool is_expression() const;
    /**
     * Returns the fully-qualified name of the type
     * being referenced.  Only safe to call when is_expression()
     * returns false.
     */
    const std::string & get_name() const;
    const JLang::context::SourceReference & get_name_source_ref() const;
    /**
     * Returns the expression representing the type
     * being referenced.  Only safe to call when is_expression()
     * returns true.
     */
    const Expression & get_expression() const;
  private:
    bool m_is_expression;
    // For raw names
    JLang::owned<Terminal> type_name;
    // for typeof expressions
    JLang::owned<Terminal> typeof_token;
    JLang::owned<Terminal> paren_l_token;
    JLang::owned<Expression> expression;
    JLang::owned<Terminal> paren_r_token;
  };

  //! Represents arguments to a template type.
  /**
   * This represents the list of arguments to a parameterized (template) type.
   * The purpose of this is to provide a list of arguments used when
   * constructing a type.  This is equivalent to the argument list of
   * a template such as std::pair<int, float> where in this context,
   * the list would consist of the types "int" and "float".  These are
   * used in the TypeSpecifierTemplate to represent the template arguments.
   */
  class TypeSpecifierCallArgs : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Constructs a new empty template argument list.
     */
    TypeSpecifierCallArgs(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
    ~TypeSpecifierCallArgs();
    /**
     * This adds the given type specifier to the list of
     * template arguments.
     */
    void add_argument(JLang::owned<TypeSpecifier> _argument);
    /**
     * This adds the next argument to the list along with accounting for
     * the comma that separates the arguments.  The comma token is
     * consumed here and is not useful for the semantics, but must be
     * consumed so that the syntax tree can accurately reproduce the output.
     */
    void add_argument(JLang::owned<Terminal> _comma_token, JLang::owned<TypeSpecifier> _argument);

    /**
     * This provides immutable access to the list of arguments.
     */
    const std::vector<JLang::owned<TypeSpecifier>> & get_arguments() const;
  private:
    std::vector<JLang::owned<Terminal>> comma_list;
    std::vector<JLang::owned<TypeSpecifier>> arguments;
  };

  //! Represents a simple type.
  /**
   * This type specifies a type along with the access qualifier
   * for the type.  For example, "const int" or "volatile float"
   * would be expressions that resolve as a type specifier.
   */
  class TypeSpecifierSimple : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Constructs a type qualified by its access semantics.
     */
    TypeSpecifierSimple(
                        JLang::owned<AccessQualifier> _access_qualifier,
                        JLang::owned<TypeName> _type_name
                        );
    /**
     * Destructor, nothing special.
     */
    ~TypeSpecifierSimple();
    /**
     * Returns the access qualifier associated with the type.
     */
    const AccessQualifier & get_access_qualifier() const;
    /**
     * Returns the name of the type being specified.
     */
    const TypeName & get_type_name() const;
  private:
    JLang::owned<AccessQualifier> access_qualifier;
    JLang::owned<TypeName> type_name;
  };

  //! Represents a type with type-arguments modifying the type's definition.
  /**
   * This is used to specify a type along with any template
   * parameters that should be passed.
   *
   * For example, in this sample, the class Foo accepts
   * one template parameter X.  The template is instantiated
   * and in this example, the TypeSpecifierTemplate
   * would represent the "Foo<int>" instance of the type.
   *
   * <pre>
   * class Foo<X> {
   * };
   *
   * Foo<int> FooWithInt;
   * </pre>
   *
   * These template arguments may be simple names or may also be
   * templates themselves as in this more complicated example:
   * 
   * <pre>
   *     Foo<Bar<u32>, u32>>
   * </pre>
   */
  class TypeSpecifierTemplate : public JLang::frontend::ast::SyntaxNode {
  public:
    TypeSpecifierTemplate(
                          JLang::owned<TypeSpecifier> _type_specifier,
                          JLang::owned<Terminal> _paren_l_token,
                          JLang::owned<TypeSpecifierCallArgs> _type_specifier_call_args,
                          JLang::owned<Terminal> _paren_r_token
                          );
    /**
     * Destructor, nothing special.
     */
    ~TypeSpecifierTemplate();
    /**
     * This returns the base type being specified.
     */
    const TypeSpecifier & get_type() const;
    /**
     * This returns the list of template parameters
     * being passed to instantiate the type.
     */
    const TypeSpecifierCallArgs & get_args() const;

  private:
    JLang::owned<TypeSpecifier> type_specifier;
    JLang::owned<Terminal> paren_l_token;
    JLang::owned<TypeSpecifierCallArgs> type_specifier_call_args;
    JLang::owned<Terminal> paren_r_token;
  };

  //! Represents a pointer to a function.
  /**
   * This type represents a pointer to a function.  This is designed to follow
   * the c-style syntax of function pointers.  This isn't the most efficient
   * or readable way to specify function pointers, but is is <em>traditional</em>
   * and well-known to those familiar with the C syntax.
   */
  class TypeSpecifierFunctionPointer : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * This represents a function pointer type as expressed by the typical
     * C syntax for function pointers:
     *
     * <pre>
     *     type_specifier ( * identifier_token )(function_definition_arg_list)
     * </pre>
     *
     * This specifies the name given by "identifier_token" as a pointer to
     * a function returning the type_specifiere and receiving the arguments
     * given.
     */
    TypeSpecifierFunctionPointer(
                                 JLang::owned<TypeSpecifier> _type_specifier,
                                 JLang::owned<Terminal> _paren_l1_token,
                                 JLang::owned<Terminal> _star_token,
                                 JLang::owned<Terminal> _identifier_token,
                                 JLang::owned<Terminal> _paren_r1_token,
                                 JLang::owned<Terminal> _paren_l2_token,
                                 JLang::owned<FunctionDefinitionArgList> _function_definition_arg_list,
                                 JLang::owned<Terminal> _paren_r2_token
                                 );
    /**
     * Destructor, nothing special.
     */
    ~TypeSpecifierFunctionPointer();
    /**
     * Returns the return type of the function pointer.
     */
    const TypeSpecifier & get_return_type() const;
    /**
     * Returns the name of the function pointer being declared.
     */
    const std::string & get_name() const;
    const JLang::context::SourceReference & get_name_source_ref() const;
    /**
     * Returns the list of arguments the function expects.
     */
    const FunctionDefinitionArgList & get_args() const;
  private:
    JLang::owned<TypeSpecifier> type_specifier;
    JLang::owned<Terminal> paren_l1_token;
    JLang::owned<Terminal> star_token;
    JLang::owned<Terminal> identifier_token;
    JLang::owned<Terminal> paren_r1_token;
    JLang::owned<Terminal> paren_l2_token;
    JLang::owned<FunctionDefinitionArgList> function_definition_arg_list;
    JLang::owned<Terminal> paren_r2_token;
  };

  //! Represents an unsafe poniter to a specific type.
  /**
   * This type represents a pointer to another type in the style
   * of C.  For example, to declare a pointer to a u32 type,
   * the following would specify that the type of 'f' should
   * be of type pointer to u32.
   *
   * <pre>
   *    u32 *f;
   * </pre>
   *
   * This type may also be qualified by its
   * access and specifies that indirection of
   * 'f' is allowed, but must not be written to
   * and must only be read from.
   *
   * <pre>
   *   u32 *const f;
   * </pre>
   *
   * Use of pointers must occur in 'unsfe' blocks
   * or functions because they carry no semantics for
   * ensuring that they are safe.
   */
  class TypeSpecifierPointerTo : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Specifies a pointer to the given type and
     * marked with access as 'const', 'volatile', or 'unspecified'.
     */
    TypeSpecifierPointerTo(
                           JLang::owned<TypeSpecifier> _type_specifier,
                           JLang::owned<Terminal> _star_token,
                           JLang::owned<AccessQualifier> _access_qualifier
                           );
    /**
     * Destructor, nothing special.
     */
    ~TypeSpecifierPointerTo();
    
    /**
     * Returns the type that is accessed behind this pointer.
     */
    const TypeSpecifier & get_type_specifier() const;
    /**
     * Returns the type of access permitted or required by
     * this pointer.
     */
    const AccessQualifier & get_access_qualifier() const;
  private:
    JLang::owned<TypeSpecifier> type_specifier;
    JLang::owned<Terminal> star_token;
    JLang::owned<AccessQualifier> access_qualifier;
  };

  //! Represents a safe reference to a specific type.
  /**
   * This represents a reference to the given type.  Similar to pointers,
   * references allow indirect access to the underlying values.  Unlike
   * pointers, however, references carry safety semantics used by the
   * borrow checker to ensure that the access is always safe.
   *
   * The syntax of references follows the pattern of C++ references, but
   * the semantics are borrowed from the Rust semantics of single ownership
   * and single mutation as well as the lifetime safety semantics.
   *
   * <pre>
   *    u32 x = 12;
   *    u32 & const f = x;   // Declares that f itself is mutable
   *                         // but the access to x through f must be read-only.
   *
   *    const u32 & f = x;   // Declares that f itself is immutable,
   *                         // but it may perform read and write operations
   *                         // on the underlying value x.
   *
   *    const u32 & const f = x; // Declares that both the reference f
   *                             // and the value it points to are read-only.
   * </pre>
   */
  class TypeSpecifierReferenceTo : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Constructs a reference to the given type
     * using the access qualifiers given.
     */
    TypeSpecifierReferenceTo(
                             JLang::owned<TypeSpecifier> _type_specifier,
                             JLang::owned<Terminal> _andpersand_token,
                             JLang::owned<AccessQualifier> _access_qualifier
                             );
    /**
     * Destructor, nothing special.
     */
    ~TypeSpecifierReferenceTo();
    /**
     * Returns the type this reference points to.
     */
    const TypeSpecifier & get_type_specifier() const;
    /**
     * Returns the expected access semantics of the
     * value this reference points to (not the access
     * semantics of this reference itself).
     */
    const AccessQualifier & get_access_qualifier() const;
  private:
    JLang::owned<TypeSpecifier> type_specifier;
    JLang::owned<Terminal> andpersand_token;
    JLang::owned<AccessQualifier> access_qualifier;
  };

  //! Represents a type being specified.
  /**
   * This represents a type being specified.  The specifics
   * of the type being represented come in several flavors
   * depending on how it is declared.
   * * TypeSpecifierSimple : 
   *     This type may be a simple type such as 'u32' or
   *     a class like 'Foo'
   * * TypeSpecifierTemplate :
   *     This is a parameterized type with arguments that
   *     define the parameters of the type.
   * * TypeSpecifierFunctionPointer :
   *     This is a pointer to a function returning a type
   *     and accepting arguments.
   * * TypeSpecifierPointerTo:
   *     This is a pointer to data of another type.
   * * TypeSpecifierReferenceTo:
   *     This is a reference to data of another type.
   *
   * Before accessing each of these types,
   * you must first check whether it holds
   * the type.  The following illustrates the
   * expected use pattern:
   *
   * <pre>
   *      const TypeSpecifier::TypeSpecifierType & type = type_specifier.get_type();
   *      if (std::holds_alternative<JLang::owned<TypeSpecifierSimple>>(type)) {
   *          const JLang::owned<TypeSpecifierSimple> & simple = std::get<JLang::owned<TypeSpecifierSimple>>(type);
   *          std::string name = simple.get_name();
   *      }
   * </pre>
   */
  class TypeSpecifier : public JLang::frontend::ast::SyntaxNode {
  public:
    typedef std::variant<
    JLang::owned<TypeSpecifierSimple>,
    JLang::owned<TypeSpecifierTemplate>,
    JLang::owned<TypeSpecifierFunctionPointer>,
    JLang::owned<TypeSpecifierPointerTo>,
    JLang::owned<TypeSpecifierReferenceTo>
    > TypeSpecifierType;
    TypeSpecifier(TypeSpecifier::TypeSpecifierType _type, const JLang::frontend::ast::SyntaxNode & _sn);
    /**
     * Destructor, nothing special.
     */
    ~TypeSpecifier();
    /**
     * Returns the specific type that this specifier refers to.
     */
    const TypeSpecifier::TypeSpecifierType & get_type() const;
  private:
    TypeSpecifier::TypeSpecifierType type;
  };

  //! Represents an argument in a function definition.
  /**
   * This represents an element of a list of arguments when defining
   * or declaring a function.  For example, in the declaration
   * of sqrt(double v), this would represent the "double v" portion
   * of that declaration consisting of the type specifier and the
   * name of the argument.
   */
  class FunctionDefinitionArg : public JLang::frontend::ast::SyntaxNode {
  public:
    FunctionDefinitionArg(JLang::owned<TypeSpecifier> _type_specifier,
                          JLang::owned<Terminal> _identifier_token
                          );
    /**
     * Destructor, nothing special.
     */
    ~FunctionDefinitionArg();
    /**
     * Represents the type of the argument.
     */
    const TypeSpecifier & get_type_specifier() const;
    /**
     * Represents the name of the argument;
     */
    const std::string & get_name() const;
    const JLang::context::SourceReference & get_name_source_ref() const;
  private:
    JLang::owned<TypeSpecifier> type_specifier;
    JLang::owned<Terminal> identifier_token;
  };

  //! Represents the list of arguments to a function definition.
  /**
   * This represents a list of argumnents to a function.  The input syntax
   * consists of a comma-separated list of arguments of type FunctionDefinitionArg.
   */
  class FunctionDefinitionArgList : public JLang::frontend::ast::SyntaxNode {
  public:
    FunctionDefinitionArgList(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
    ~FunctionDefinitionArgList();
    /**
     * Returns the list of arguments to the function.
     */
    const std::vector<JLang::owned<FunctionDefinitionArg>> & get_arguments() const;
    /**
     * Adds a single argument to the function, used during
     * the parse stage to construct the function arguments
     * from the input syntax.
     */
    void add_argument(JLang::owned<FunctionDefinitionArg> _argument);
    /**
     * Adds the comma separator to the function arguments, but this
     * plays no role in the semantics.
     */
    void add_comma(JLang::owned<Terminal> _comma);
  private:
    std::vector<JLang::owned<Terminal>> commas;
    std::vector<JLang::owned<FunctionDefinitionArg>> arguments;
  };

  //! Represents the declaration of a function.
  /**
   * This represents the declaration of a function (i.e. without the body), specifying only
   * the return value, name, and arguments.
   * This represents, for example, a declaration of the form:
   * <pre>
   *    public unsafe f32 pow(f32 base, f32 exponent);
   * </pre>
   */
  class FileStatementFunctionDeclaration : public JLang::frontend::ast::SyntaxNode {
  public:
    FileStatementFunctionDeclaration(
                                     JLang::owned<AccessModifier> _access_modifier,
                                     JLang::owned<UnsafeModifier> _unsafe_modifier,
                                     JLang::owned<TypeSpecifier> _type_specifier,
                                     JLang::owned<Terminal> _name,
                                     JLang::owned<Terminal> _paren_l,
                                     JLang::owned<FunctionDefinitionArgList> _arguments,
                                     JLang::owned<Terminal> _paren_r,
                                     JLang::owned<Terminal> _semicolon
                                     );
    /**
     * Destructor, nothing special.
     */
    ~FileStatementFunctionDeclaration();
    /**
     * Represents the type of access to the function.
     */
    const AccessModifier & get_access_modifier() const;
    /**
     * Represents a declaration that the function is
     * marked as safe or unsafe.
     */
    const UnsafeModifier & get_unsafe_modifier() const;
    /**
     * Specifies the type of the return value of the function.
     */
    const TypeSpecifier & get_return_type() const;
    /**
     * Specifies the name of the function.
     */
    const std::string & get_name() const;
    const JLang::context::SourceReference & get_name_source_ref() const;
    /**
     * Specifies the list of arguments with names
     * and types for the function.
     */
    const FunctionDefinitionArgList & get_arguments() const;
    
  private:
    JLang::owned<AccessModifier> access_modifier;
    JLang::owned<UnsafeModifier> unsafe_modifier;
    JLang::owned<TypeSpecifier> type_specifier;
    JLang::owned<Terminal> name; // function name (IDENTIFIER)
    JLang::owned<Terminal> paren_l; // argument list delimiter PAREN_L
    JLang::owned<FunctionDefinitionArgList> arguments;
    JLang::owned<Terminal> paren_r; // argument list delimiter PAREN_R
    JLang::owned<Terminal> semicolon; // argument list delimiter SEMICOLON
  };

  //! Represents the declaration of a variable inside the scope of a function or block.
  /**
   * This specifies the declaration of a variable inside the body of a function
   * or other block.  For example, the following shows the declaration of a variable
   * of type 'u32' to the variable 'x' as being constant and initializes the
   * value to '0' and also defines a buffer of 32 character (u8) values.
   *
   * <pre>
   * void foo()
   * {
   *     const u32 x = 0;
   *     u8 buffer[32];
   * }
   * </pre>
   */
  class StatementVariableDeclaration : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Used to create a variable declaration during the parse stage,
     * consuming the parse tokens.
     */
    StatementVariableDeclaration(
                                 JLang::owned<TypeSpecifier> _type_specifier,
                                 JLang::owned<Terminal> _identifier_token,
                                 JLang::owned<ArrayLength> _array_length,
                                 JLang::owned<GlobalInitializer> _global_initializer,
                                 JLang::owned<Terminal> _semicolon_token
                                 );
    /**
     * Destructor, nothing special.
     */
    ~StatementVariableDeclaration();
    /**
     * Returns the type of the variable being declared.
     */
    const TypeSpecifier & get_type_specifier() const;
    /**
     * Returns the name of the variable being declared.
     */
    const std::string & get_name() const;
    const JLang::context::SourceReference & get_name_source_ref() const;
    /**
     * If the variable is an array, this gives the length
     * of the array being declared.
     */
    const ArrayLength & get_array_length() const;
    /**
     * Returns the initializer expression, usually a literal,
     * that is used to populate the variable with known values.
     */
    const GlobalInitializer & get_initializer() const;
  private:
    JLang::owned<TypeSpecifier> type_specifier;
    JLang::owned<Terminal> identifier_token;
    JLang::owned<ArrayLength> array_length;
    JLang::owned<GlobalInitializer> global_initializer;
    JLang::owned<Terminal> semicolon_token;
  private:
  };

  //! Represents a block of statements in a particular scope.
  /**
   * This statement consists of a block of statements in a particular
   * scope.  It may be optionally marked as unsafe, indicating that
   * it may contain raw pointers and other potentially unsafe
   * semantics.
   *
   * For example, this is a scope block defined as unsafe
   * because is makes use of potentially unsafe semantics
   * such as raw pointer indirection.
   *
   * <pre>
   * void foo()
   * {
   *     u32 q;
   *     unsafe {
   *         u32 *x = &q;
   *         *x = 'p';
   *     }
   * }
   * </pre>
   */
  class StatementBlock : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Used in the parse stage to create a block of statements
     * with an optional unsafe modifier.
     */
    StatementBlock(
                   JLang::owned<UnsafeModifier> _unsafe_modifier,
                   JLang::owned<ScopeBody> _scope_body
                   );
    /**
     * Destructor, nothing special.
     */
    ~StatementBlock();
    /**
     * Returns the unsafe modifier associated with the
     * block.
     */
    const UnsafeModifier & get_unsafe_modifier() const;
    /**
     * Returns the scope body containing the statements
     * defined by the scope.
     */
    const ScopeBody & get_scope_body() const;
  private:
    JLang::owned<UnsafeModifier> unsafe_modifier;
    JLang::owned<ScopeBody> scope_body;
  private:
  };

  //! This represents a statement in a function or scope that computes an expression.
  /**
   * Statements may contain expressions to be evaluated.  For example, a statement may
   * consist of an assignment or a call to a function as shown in this example.
   * <pre>
   * void foo()
   * {
   *     u32 p = 4;
   *     u32 q = 16;
   *     u32 x;
   *     x = p + q; // Assignment expression with addition expression.
   *     bar(x);    // Function-call expression.
   * }
   * </pre>
   */
  class StatementExpression : public JLang::frontend::ast::SyntaxNode {
  public:
    StatementExpression(
                        JLang::owned<Expression> _expression,
                        JLang::owned<Terminal> _semicolon_token
                        );
    /**
     * Destructor, nothing special.
     */
    ~StatementExpression();
    const Expression & get_expression() const;
  private:
    JLang::owned<Expression> expression;
    JLang::owned<Terminal> semicolon_token;
  };

  //! This represents an if condition in one of several forms.
  /**
   * The if/else construct in JLang comes in several forms.  It is
   * designed slightly differently than the C equivalent in order
   * to ensure that it does not have the "dangling else" ambiguity.
   * The curly-braces are mandatory as a part of eliminating
   * that ambiguity.  The only excption to this case is the 'else if'
   * construct which does NOT require curly braces after the 'else'
   * and allows very familiar semantics.
   *
   * The following are examples of the various forms that the
   * if construct may take.
   *
   * If without else:
   * <pre>
   * ...
   * if (foo) {
   *    bar();
   * }
   * ...
   * </pre>
   *
   * If with else.  Note that in this case, the curly braces '{' are
   * mandatory for both the if and else blocks.
   * <pre>
   * ...
   * if (foo) {
   *    bar();
   * }
   * else {
   *    baz();
   * }
   * ...
   * </pre>
   *
   * If/else if/else if chain.  Note that ONLY in this case,
   * we are allowed to skip the curly brace between the 'else' and
   * the next 'if', allowing the semantics to be unambiguious.
   *
   * <pre>
   * ...
   * if (foo) {
   *    bar();
   * }
   * else if (q) {
   *    baz();
   * }
   * else {
   *    end();
   * }
   * ...
   * </pre>
   *
   *
   */
  class StatementIfElse : public JLang::frontend::ast::SyntaxNode {
  public:
    /**
     * Constructs an 'if' statement with a single block
     * for when the condition is true.
     */
    StatementIfElse(
                    JLang::owned<Terminal> _if_token,
                    JLang::owned<Terminal> _paren_l_token,
                    JLang::owned<Expression> _expression,
                    JLang::owned<Terminal> _paren_r_token,
                    JLang::owned<ScopeBody> _if_scope_body
                    );
    /**
     * Constructs an if/else block where one block
     * represents the expression true and the other
     * block represents the code for the expression false.
     */
    StatementIfElse(
                    JLang::owned<Terminal> _if_token,
                    JLang::owned<Terminal> _paren_l_token,
                    JLang::owned<Expression> _expression,
                    JLang::owned<Terminal> _paren_r_token,
                    JLang::owned<ScopeBody> _if_scope_body,
                    JLang::owned<Terminal> _else_token,
                    JLang::owned<ScopeBody> _else_scope_body
                    );
    /**
     * Constructs an if/elseif chain where each 'else if'
     * can be modelled as the next element of a chain of
     * conditions.
     */
    StatementIfElse(
                    JLang::owned<Terminal> _if_token,
                    JLang::owned<Terminal> _paren_l_token,
                    JLang::owned<Expression> _expression,
                    JLang::owned<Terminal> _paren_r_token,
                    JLang::owned<ScopeBody> _if_scope_body,
                    JLang::owned<Terminal> _else_token,
                    JLang::owned<StatementIfElse> _statement_if_else
                    );
    /**
     * Destructor, nothing special.
     */
    ~StatementIfElse();
    /**
     * This is the expression that is evaluated
     * when the if statement is reached.  It must
     * have the type of 'boolean' and governs what
     * happens during execution.
     */
    const Expression & get_expression() const;
    /**
     * This represents the scope body to execute
     * when the expression evaluates to true.
     */
    const ScopeBody & get_if_scope_body() const;
    /**
     * If ths statement is an 'else if' block, this
     * represents the 'next' if block to execute in
     * the chain, so another condition and another set
     * of blocks can be specified.
     */
    const StatementIfElse & get_else_if() const;
    /**
     * In an 'else' block, this represents the
     * scope body to execute when the expression
     * evaluates to 'false'.
     */
    const ScopeBody & get_else_scope_body() const;
    bool has_else() const;
    bool has_else_if() const;
  private:
    bool m_has_else;
    bool m_has_else_if;
    JLang::owned<Terminal> if_token;
    JLang::owned<Terminal> paren_l_token;
    JLang::owned<Expression> expression;
    JLang::owned<Terminal> paren_r_token;
    JLang::owned<ScopeBody> if_scope_body;
    JLang::owned<Terminal> else_token;
    JLang::owned<ScopeBody> else_scope_body;
    JLang::owned<StatementIfElse> else_if;
  };

  //! Represents a 'while' statement in a function or scope.
  /**
   * This represents a 'while' statement indicating that
   * the specified scope block should be executed as long
   * as the given boolean expression evaluates to 'true'.
   * This represents the typical pattern:
   *
   * <pre>
   *    boolean running = true;
   *    while (running) {
   *        if (input()) {
   *            running = false;
   *        }
   *    }
   * </pre>
   */
  class StatementWhile : public JLang::frontend::ast::SyntaxNode {
  public:
    StatementWhile(
                   JLang::owned<Terminal> _while_token,
                   JLang::owned<Terminal> _paren_l_token,
                   JLang::owned<Expression> _expression,
                   JLang::owned<Terminal> _paren_r_token,
                   JLang::owned<ScopeBody> _scope_body
                   );
    /**
     * Destructor, nothing special.
     */
    ~StatementWhile();
    /**
     * This is the expression that is evaluated each time
     * the loop is evaluated.  If this expression returns 'true'
     * then the scope is executed and the expression is evaluated
     * again.  This happens until the expression evaluates to 'false'.
     */
    const Expression & get_expression() const;
    const ScopeBody & get_scope_body() const;
  private:
    JLang::owned<Terminal> while_token;
    JLang::owned<Terminal> paren_l_token;
    JLang::owned<Expression> expression;
    JLang::owned<Terminal> paren_r_token;
    JLang::owned<ScopeBody> scope_body;
  };
  
  class StatementFor : public JLang::frontend::ast::SyntaxNode {
  public:
    StatementFor(
                 JLang::owned<Terminal> _for_token,
                 JLang::owned<Terminal> _paren_l_token,
                 JLang::owned<Expression> _expression_initial,
                 JLang::owned<Terminal> _semicolon_initial,
                 JLang::owned<Expression> _expression_termination,
                 JLang::owned<Terminal> _semicolon_termination,
                 JLang::owned<Expression> _expression_increment,
                 JLang::owned<Terminal> _paren_r_token,
                 JLang::owned<ScopeBody> _scope_body
                 );
    /**
     * Destructor, nothing special.
     */
    ~StatementFor();
    const Expression & get_expression_initial() const;
    const Expression & get_expression_termination() const;
    const Expression & get_expression_increment() const;
    const ScopeBody & get_scope_body() const;
  private:
    JLang::owned<Terminal> for_token;
    JLang::owned<Terminal> paren_l_token;
    JLang::owned<Expression> expression_initial;
    JLang::owned<Terminal> semicolon_initial;
    JLang::owned<Expression> expression_termination;
    JLang::owned<Terminal> semicolon_termination;
    JLang::owned<Expression> expression_increment;
    JLang::owned<Terminal> paren_r_token;
    JLang::owned<ScopeBody> scope_body;
  };

    class StatementSwitchBlock : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitchBlock(
                           JLang::owned<Terminal> _default_token,
                           JLang::owned<Terminal> _colon_token,
                           JLang::owned<ScopeBody> _scope_body
                           );
      StatementSwitchBlock(
                           JLang::owned<Terminal> _case_token,
                           JLang::owned<Expression> _expression,
                           JLang::owned<Terminal> _colon_token,
                           JLang::owned<ScopeBody> _scope_body
                           );
    /**
     * Destructor, nothing special.
     */
      ~StatementSwitchBlock();
      bool is_default() const;
      const Expression & get_expression();
      const ScopeBody & get_scope_body();
    private:
      bool m_is_default;
      // For the default case
      JLang::owned<Terminal> default_token;
      // For the expression case
      JLang::owned<Terminal> case_token;
      JLang::owned<Expression> expression;
      // Common:
      JLang::owned<Terminal> colon_token;
      JLang::owned<ScopeBody> scope_body;
    };
    
    class StatementSwitchContent : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitchContent(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
      ~StatementSwitchContent();
      const std::vector<JLang::owned<StatementSwitchBlock>> & get_blocks() const;
      void add_block(JLang::owned<StatementSwitchBlock> _block);
    private:
      std::vector<JLang::owned<StatementSwitchBlock>> blocks;
    };
    
    class StatementSwitch : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementSwitch(
                      JLang::owned<Terminal> _switch_token,
                      JLang::owned<Terminal> _paren_l_token,
                      JLang::owned<Expression> expression,
                      JLang::owned<Terminal> _paren_r_token,
                      JLang::owned<Terminal> _brace_l_token,
                      JLang::owned<StatementSwitchContent> _switch_content,
                      JLang::owned<Terminal> _brace_r_token
                      );
    /**
     * Destructor, nothing special.
     */
      ~StatementSwitch();
      const Expression & get_expression() const;
      const StatementSwitchContent & get_switch_content() const;
    private:
      JLang::owned<Terminal> switch_token;
      JLang::owned<Terminal> paren_l_token;
      JLang::owned<Expression> expression;
      JLang::owned<Terminal> paren_r_token;
      JLang::owned<Terminal> brace_l_token;
      JLang::owned<StatementSwitchContent> switch_content;
      JLang::owned<Terminal> brace_r_token;
    };
    class StatementLabel : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementLabel(
                     JLang::owned<Terminal> _label_token,
                     JLang::owned<Terminal> _identifier_token,
                     JLang::owned<Terminal> _colon_token
                     );
    /**
     * Destructor, nothing special.
     */
      ~StatementLabel();
      const std::string & get_name() const;
      const JLang::context::SourceReference & get_name_source_ref() const;
    private:
      JLang::owned<Terminal> label_token;
      JLang::owned<Terminal> identifier_token;
      JLang::owned<Terminal> colon_token;
    };
    class StatementGoto : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementGoto(
                    JLang::owned<Terminal> _goto_token,
                    JLang::owned<Terminal> _identifier_token,
                    JLang::owned<Terminal> _semicolon_token
                    );
    /**
     * Destructor, nothing special.
     */
      ~StatementGoto();
      const std::string & get_label() const;
      const JLang::context::SourceReference & get_label_source_ref() const;
    private:
      JLang::owned<Terminal> goto_token;
      JLang::owned<Terminal> identifier_token;
      JLang::owned<Terminal> semicolon_token;
    };
    class StatementBreak : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementBreak(
                     JLang::owned<Terminal> _break_token,
                     JLang::owned<Terminal> _semicolon_token
                     );
    /**
     * Destructor, nothing special.
     */
      ~StatementBreak();
    private:
      JLang::owned<Terminal> break_token;
      JLang::owned<Terminal> semicolon_token;
    };
    class StatementContinue : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementContinue(
                        JLang::owned<Terminal> _continue_token,
                        JLang::owned<Terminal> _semicolon_token
                        );
    /**
     * Destructor, nothing special.
     */
      ~StatementContinue();
    private:
      JLang::owned<Terminal> continue_token;
      JLang::owned<Terminal> semicolon_token;
    };
    class StatementReturn : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementReturn(
                      JLang::owned<Terminal> _return_token,
                      JLang::owned<Expression> _expression,
                      JLang::owned<Terminal> _semicolon_token
                      );
    /**
     * Destructor, nothing special.
     */
      ~StatementReturn();
      const Expression & get_expression() const;
    private:
      JLang::owned<Terminal> return_token;
      JLang::owned<Expression> expression;
      JLang::owned<Terminal> semicolon_token;
    };

    class Statement : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
            JLang::owned<StatementVariableDeclaration>,
            JLang::owned<StatementBlock>,
            JLang::owned<StatementExpression>,
            JLang::owned<StatementIfElse>,
            JLang::owned<StatementWhile>,
            JLang::owned<StatementFor>,
            JLang::owned<StatementSwitch>,
            JLang::owned<StatementLabel>,
            JLang::owned<StatementGoto>,
            JLang::owned<StatementBreak>,
            JLang::owned<StatementContinue>,
            JLang::owned<StatementReturn>
      > StatementType;

      Statement(StatementType _statement, const SyntaxNode & _sn);
    /**
     * Destructor, nothing special.
     */
      ~Statement();
      const StatementType & get_statement() const;
    private:
      StatementType statement;
    };
    
    class StatementList : public JLang::frontend::ast::SyntaxNode {
    public:
      StatementList(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
      ~StatementList();
      void add_statement(JLang::owned<Statement> _statement);
      const std::vector<JLang::owned<Statement>> &get_statements() const;
    private:
      std::vector<JLang::owned<Statement>> statements;
    };
    
    class ScopeBody : public JLang::frontend::ast::SyntaxNode {
    public:
      ScopeBody(
                JLang::owned<Terminal> brace_l_token,
                JLang::owned<StatementList> statement_list,
                JLang::owned<Terminal> brace_r_token
                );
    /**
     * Destructor, nothing special.
     */
      ~ScopeBody();
      const StatementList & get_statements() const;
    private:
      JLang::owned<Terminal> brace_l_token;
      JLang::owned<StatementList> statement_list;
      JLang::owned<Terminal> brace_r_token;
    };
    
    class FileStatementFunctionDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementFunctionDefinition(
                                      JLang::owned<AccessModifier> _access_modifier,
                                      JLang::owned<UnsafeModifier> _unsafe_modifier,
                                      JLang::owned<TypeSpecifier> _type_specifier,
                                      JLang::owned<Terminal> _name,
                                      JLang::owned<Terminal> _paren_l,
                                      JLang::owned<FunctionDefinitionArgList> _arguments,
                                      JLang::owned<Terminal> _paren_r,
                                      JLang::owned<ScopeBody> _scope_body
                                      );
    /**
     * Destructor, nothing special.
     */
      ~FileStatementFunctionDefinition();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const Terminal & get_name() const;
      const FunctionDefinitionArgList & get_arguments() const;
      const ScopeBody & get_scope_body() const;
      
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<UnsafeModifier> unsafe_modifier;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> name; // function name (IDENTIFIER)
      JLang::owned<Terminal> paren_l; // argument list delimiter PAREN_L
      JLang::owned<FunctionDefinitionArgList> arguments;
      JLang::owned<Terminal> paren_r; // argument list delimiter PAREN_R
      JLang::owned<ScopeBody> scope_body; // argument list delimiter SEMICOLON
    };

    class ArrayLength : public JLang::frontend::ast::SyntaxNode {
    public:
      ArrayLength(const JLang::context::SourceReference & _source_ref);
      ArrayLength(
                  JLang::owned<Terminal> _bracket_l_token,
                  JLang::owned<Terminal> _literal_int_token,
                  JLang::owned<Terminal> _bracket_r_token
                  );
    /**
     * Destructor, nothing special.
     */
      ~ArrayLength();
      bool is_array() const;
      size_t get_size() const;
      const JLang::context::SourceReference & get_size_source_ref() const;
    private:
      JLang::owned<Terminal> bracket_l_token;
      JLang::owned<Terminal> literal_int_token;
      JLang::owned<Terminal> bracket_r_token;
    };

    class ClassDeclStart : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassDeclStart(
                     JLang::owned<AccessModifier> _access_modifier,
                     JLang::owned<Terminal> _class_token,
                     JLang::owned<Terminal> _identifier_token,
                     JLang::owned<ClassArgumentList> _class_argument_list,
                     bool is_identifier
                     );
                     
    /**
     * Destructor, nothing special.
     */
      ~ClassDeclStart();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const JLang::context::SourceReference & get_name_source_ref() const;

      const ClassArgumentList & get_argument_list() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<Terminal> class_token;
      JLang::owned<Terminal> identifier_token;
      JLang::owned<ClassArgumentList> class_argument_list;
      std::string name;
    };
    class ClassArgumentList : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassArgumentList(const JLang::context::SourceReference & _source_ref);
      ClassArgumentList(JLang::owned<Terminal> _argument);
    /**
     * Destructor, nothing special.
     */
      ~ClassArgumentList();
      void add_argument(JLang::owned<Terminal> _comma, JLang::owned<Terminal> _argument);
      void add_parens(JLang::owned<Terminal> _paren_l, JLang::owned<Terminal> _paren_r);
      const std::vector<JLang::owned<Terminal>> & get_arguments() const;
    private:
      JLang::owned<Terminal> paren_l;
      std::vector<JLang::owned<Terminal>> comma_list;
      std::vector<JLang::owned<Terminal>> argument_list;
      JLang::owned<Terminal> paren_r;
    };

    class ClassMemberDeclarationVariable : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationVariable(
                                     JLang::owned<AccessModifier> _access_modifier,
                                     JLang::owned<TypeSpecifier> _type_specifier,
                                     JLang::owned<Terminal> _identifier_token,
                                     JLang::owned<ArrayLength> _array_length,
                                     JLang::owned<Terminal> _semicolon_token
                                     );
    /**
     * Destructor, nothing special.
     */
      ~ClassMemberDeclarationVariable();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const JLang::context::SourceReference & get_name_source_ref() const;
      const ArrayLength & get_array_length() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> identifier_token;
      JLang::owned<ArrayLength> array_length;
      JLang::owned<Terminal> semicolon_token;
    };    
    class ClassMemberDeclarationMethod : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationMethod(
                                     JLang::owned<AccessModifier> _access_modifier,
                                     JLang::owned<TypeSpecifier> _type_specifier,
                                     JLang::owned<Terminal> _identifier_token,
                                     JLang::owned<Terminal> _paren_l_token,
                                     JLang::owned<FunctionDefinitionArgList> _function_definition_arg_list,
                                     JLang::owned<Terminal> _paren_r_token,
                                     JLang::owned<Terminal> _semicolon_token
                                   );
    /**
     * Destructor, nothing special.
     */
      ~ClassMemberDeclarationMethod();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const JLang::context::SourceReference & get_name_source_ref() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> identifier_token;
      JLang::owned<Terminal> paren_l_token;
      JLang::owned<FunctionDefinitionArgList> function_definition_arg_list;
      JLang::owned<Terminal> paren_r_token;
      JLang::owned<Terminal> semicolon_token;
    };
    class ClassMemberDeclarationConstructor : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationConstructor(
                                        JLang::owned<AccessModifier> _access_modifier,
                                        JLang::owned<TypeSpecifier> _type_specifier,
                                        JLang::owned<Terminal> _paren_l_token,
                                        JLang::owned<FunctionDefinitionArgList> _function_definition_arg_list,
                                        JLang::owned<Terminal> _paren_r_token,
                                        JLang::owned<Terminal> _semicolon_token
                                        );
    /**
     * Destructor, nothing special.
     */
      ~ClassMemberDeclarationConstructor();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> paren_l_token;
      JLang::owned<FunctionDefinitionArgList> function_definition_arg_list;
      JLang::owned<Terminal> paren_r_token;
      JLang::owned<Terminal> semicolon_token;
    };
    class ClassMemberDeclarationDestructor : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationDestructor(
                                       JLang::owned<AccessModifier> _access_modifier,
                                       JLang::owned<Terminal> _tilde_token,
                                       JLang::owned<TypeSpecifier> _type_specifier,
                                       JLang::owned<Terminal> _paren_l_token,
                                       JLang::owned<FunctionDefinitionArgList> _function_definition_arg_list,
                                       JLang::owned<Terminal> _paren_r_token,
                                       JLang::owned<Terminal> _semicolon_token
                                   );
    /**
     * Destructor, nothing special.
     */
      ~ClassMemberDeclarationDestructor();
      const AccessModifier & get_access_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const FunctionDefinitionArgList & get_arguments() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<Terminal> tilde_token;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> paren_l_token;
      JLang::owned<FunctionDefinitionArgList> function_definition_arg_list;
      JLang::owned<Terminal> paren_r_token;
      JLang::owned<Terminal> semicolon_token;
    };
    
    class ClassMemberDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        JLang::owned<ClassMemberDeclarationVariable>,
        JLang::owned<ClassMemberDeclarationMethod>,
        JLang::owned<ClassMemberDeclarationConstructor>,
        JLang::owned<ClassMemberDeclarationDestructor>,
        JLang::owned<ClassDeclaration>,
        JLang::owned<ClassDefinition>,
        JLang::owned<EnumDefinition>,
        JLang::owned<TypeDefinition>
      > MemberType;
      ClassMemberDeclaration(
                             MemberType _member,
                             const JLang::frontend::ast::SyntaxNode & _sn
                             );
    /**
     * Destructor, nothing special.
     */
      ~ClassMemberDeclaration();
      const ClassMemberDeclaration::MemberType & get_member();
    private:
      MemberType member;
    };
    
    class ClassMemberDeclarationList : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassMemberDeclarationList(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
      ~ClassMemberDeclarationList();
      const std::vector<JLang::owned<ClassMemberDeclaration>> & get_members() const;
      void add_member(JLang::owned<ClassMemberDeclaration> _member);
    private:
      std::vector<JLang::owned<ClassMemberDeclaration>> members;
    };

  /**
   * This is a forward declaration of a class which declares the
   * existence of a class, but does not specify the details of it.
   */
  class ClassDeclaration : public JLang::frontend::ast::SyntaxNode {
  public:
    ClassDeclaration(
                     JLang::owned<ClassDeclStart> _class_decl_start,
                     JLang::owned<Terminal> _semicolon_token
                     );
    /**
     * Destructor, nothing special.
     */
    ~ClassDeclaration();
    const AccessModifier & get_access_modifier() const;
    
    const std::string & get_name() const;
    const JLang::context::SourceReference & get_name_source_ref() const;
    
    const ClassArgumentList & get_argument_list() const;
  private:
    JLang::owned<ClassDeclStart> class_decl_start;
    JLang::owned<Terminal> semicolon_token;
  };
  
    class ClassDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      ClassDefinition(
                      JLang::owned<ClassDeclStart> _class_decl_start,
                      JLang::owned<Terminal> _brace_l_token,
                      JLang::owned<ClassMemberDeclarationList> _class_member_declaration_list,
                      JLang::owned<Terminal> _brace_r_token,
                      JLang::owned<Terminal> _semicolon_token
                      );
    /**
     * Destructor, nothing special.
     */
      ~ClassDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const JLang::context::SourceReference & get_name_source_ref() const;
      const ClassArgumentList & get_argument_list() const;
      const std::vector<JLang::owned<ClassMemberDeclaration>> & get_members() const;
    private:
      JLang::owned<ClassDeclStart> class_decl_start;
      JLang::owned<Terminal> brace_l_token;
      JLang::owned<ClassMemberDeclarationList> class_member_declaration_list;
      JLang::owned<Terminal> brace_r_token;
      JLang::owned<Terminal> semicolon_token;
    };

    class TypeDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      TypeDefinition(
                     JLang::owned<AccessModifier> _access_modifier,
                     JLang::owned<Terminal> _typedef_token,
                     JLang::owned<TypeSpecifier> _type_specifier,
                     JLang::owned<Terminal> _identifier_token,
                     JLang::owned<Terminal> _semicolon_token
                     );
    /**
     * Destructor, nothing special.
     */
      ~TypeDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & get_name() const;
      const JLang::context::SourceReference & get_name_source_ref() const;
      const TypeSpecifier & get_type_specifier() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<Terminal> typedef_token;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> identifier_token;
      JLang::owned<Terminal> semicolon_token;
    };

    class EnumDefinitionValue : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinitionValue(
                          JLang::owned<Terminal> _identifier_token,
                          JLang::owned<Terminal> _equals_token,
                          JLang::owned<ExpressionPrimary> _expression_primary,
                          JLang::owned<Terminal> _semicolon_token
                          );                          
    /**
     * Destructor, nothing special.
     */
      ~EnumDefinitionValue();
      const std::string & get_name() const;
      const JLang::context::SourceReference & get_name_source_ref() const;
      const ExpressionPrimary & get_expression() const;
    private:
      JLang::owned<Terminal> identifier_token;
      JLang::owned<Terminal> equals_token;
      JLang::owned<ExpressionPrimary> expression_primary;
      JLang::owned<Terminal> semicolon_token;      
    };
    
    class EnumDefinitionValueList : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinitionValueList(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
      ~EnumDefinitionValueList();
      void add_value(JLang::owned<EnumDefinitionValue>);
      const std::vector<JLang::owned<EnumDefinitionValue>> &get_values() const;
    private:
      std::vector<JLang::owned<EnumDefinitionValue>> values;
    };

    class EnumDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      EnumDefinition(
                     JLang::owned<AccessModifier> _access_modifier,
                     JLang::owned<Terminal> _enum_token,
                     JLang::owned<Terminal> _type_name_token,
                     JLang::owned<Terminal> _identifier_token,
                     JLang::owned<Terminal> _brace_l_token,
                     JLang::owned<EnumDefinitionValueList> _enum_value_list,
                     JLang::owned<Terminal> _brace_r_token,
                     JLang::owned<Terminal> _semicolon_token
                     );
    /**
     * Destructor, nothing special.
     */
      ~EnumDefinition();
      const AccessModifier & get_access_modifier() const;
      const std::string & type_name() const;
      const JLang::context::SourceReference & get_type_name_source_ref() const;
      
      const std::string & enum_name() const;
      const JLang::context::SourceReference & get_enum_name_source_ref() const;
      
      const EnumDefinitionValueList & get_value_list() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<Terminal> enum_token;
      JLang::owned<Terminal> type_name_token;
      JLang::owned<Terminal> identifier_token;
      JLang::owned<Terminal> brace_l_token;
      JLang::owned<EnumDefinitionValueList> enum_value_list;
      JLang::owned<Terminal> brace_r_token;
      JLang::owned<Terminal> semicolon_token;
      
    };

    class ExpressionPrimaryIdentifier : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryIdentifier(JLang::owned<Terminal> _identifier_token);
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPrimaryIdentifier();
      const std::string & get_identifier() const;
      const JLang::context::SourceReference & get_identifier_source_ref() const;
    private:
      JLang::owned<Terminal> identifier_token;
    };
    class ExpressionPrimaryNested : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryNested(
                              JLang::owned<Terminal> _paren_l_token,
                              JLang::owned<Expression> _expression,
                              JLang::owned<Terminal> _paren_r_token
                              );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPrimaryNested();
      const Expression & get_expression() const;
    private:
      JLang::owned<Terminal> paren_l_token;
      JLang::owned<Expression> expression;
      JLang::owned<Terminal> paren_r_token;
    };

    class ExpressionPrimaryLiteralInt : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralInt(
                                  JLang::owned<Terminal> literal_token
                                  );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPrimaryLiteralInt();
      const std::string & get_value() const;
      const JLang::context::SourceReference & get_value_source_ref() const;
    private:
      JLang::owned<Terminal> literal_token;
    };
    class ExpressionPrimaryLiteralChar : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralChar(
                                  JLang::owned<Terminal> _literal_token
                                  );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPrimaryLiteralChar();
      const std::string & get_value() const;
      const JLang::context::SourceReference & get_value_source_ref() const;
    private:
      JLang::owned<Terminal> literal_token;
    };
    class ExpressionPrimaryLiteralString : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralString(
                                     JLang::owned<Terminal> _literal_token
                                     );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPrimaryLiteralString();
      const std::string & get_value() const;
      const JLang::context::SourceReference & get_value_source_ref() const;
    private:
      JLang::owned<Terminal> literal_token;
    };
    class ExpressionPrimaryLiteralFloat : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPrimaryLiteralFloat(
                                  JLang::owned<Terminal> _literal_token
                                  );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPrimaryLiteralFloat();
      const std::string & get_value() const;
      const JLang::context::SourceReference & get_value_source_ref() const;
    private:
      JLang::owned<Terminal> literal_token;
    };
    
    class ExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<JLang::owned<ExpressionPrimaryIdentifier>,
                           JLang::owned<ExpressionPrimaryNested>,
                           JLang::owned<ExpressionPrimaryLiteralChar>,
                           JLang::owned<ExpressionPrimaryLiteralString>,
                           JLang::owned<ExpressionPrimaryLiteralInt>,
                           JLang::owned<ExpressionPrimaryLiteralFloat>
                           > ExpressionType;
      ExpressionPrimary(ExpressionPrimary::ExpressionType _expression_type, const SyntaxNode & _sn);
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPrimary();
      const ExpressionPrimary::ExpressionType & get_expression() const;
    private:
      ExpressionPrimary::ExpressionType expression_type;
    };

    class ExpressionPostfixArrayIndex : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixArrayIndex(
                                  JLang::owned<Expression> _array_expression,
                                  JLang::owned<Terminal> _bracket_l_token,
                                  JLang::owned<Expression> _index_expression,
                                  JLang::owned<Terminal> _bracket_r_token
                                  );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPostfixArrayIndex();
      const Expression & get_array() const;
      const Expression & get_index() const;
    private:
      JLang::owned<Expression> array_expression;
      JLang::owned<Terminal> bracket_l_token;
      JLang::owned<Expression> index_expression;
      JLang::owned<Terminal> bracket_r_token;
    };

    class ArgumentExpressionList : public JLang::frontend::ast::SyntaxNode {
    public:
      ArgumentExpressionList(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
      ~ArgumentExpressionList();
      const std::vector<JLang::owned<Expression>> & get_arguments() const;
      void add_argument(JLang::owned<Expression> _argument);
      void add_argument(JLang::owned<Terminal> _comma_token, JLang::owned<Expression> _argument);
    private:
      std::vector<JLang::owned<Terminal>> comma_list;
      std::vector<JLang::owned<Expression>> arguments;
    };
    
    class ExpressionPostfixFunctionCall : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixFunctionCall(
                                    JLang::owned<Expression> _function_expression,
                                    JLang::owned<Terminal> _paren_l_token,
                                    JLang::owned<ArgumentExpressionList> _arguments,
                                    JLang::owned<Terminal> _paren_r_token
                                    );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPostfixFunctionCall();
      const Expression & get_function() const;
      const ArgumentExpressionList & get_arguments() const;
    private:
      JLang::owned<Expression> function_expression;
      JLang::owned<Terminal> paren_l_token;
      JLang::owned<ArgumentExpressionList> arguments;
      JLang::owned<Terminal> paren_r_token;
    };

    class ExpressionPostfixDot : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixDot(
                           JLang::owned<Expression> _expression,
                           JLang::owned<Terminal> _dot_token,
                           JLang::owned<Terminal> _identifier_token
                           );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPostfixDot();
      const Expression & get_expression() const;
      const std::string & get_identifier() const;
      const JLang::context::SourceReference & get_identifier_source_ref() const;
    private:
      JLang::owned<Expression> expression;
      JLang::owned<Terminal> dot_token;
      JLang::owned<Terminal> identifier_token;
    };
    
    class ExpressionPostfixArrow : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionPostfixArrow(
                             JLang::owned<Expression> _expression,
                             JLang::owned<Terminal> _arrow_token,
                             JLang::owned<Terminal> _identifier_token
                             );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPostfixArrow();
      const Expression & get_expression() const;
      const std::string & get_identifier() const;
      const JLang::context::SourceReference & get_identifier_source_ref() const;
    private:
      JLang::owned<Expression> expression;
      JLang::owned<Terminal> arrow_token;
      JLang::owned<Terminal> identifier_token;
    };

    class ExpressionPostfixIncDec : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef enum {
        INCREMENT,
        DECREMENT
      } OperationType;
      ExpressionPostfixIncDec(
                              JLang::owned<Expression> _expression,
                              JLang::owned<Terminal> _operator_token,
                              ExpressionPostfixIncDec::OperationType _type                              
                              );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionPostfixIncDec();
      const ExpressionPostfixIncDec::OperationType & get_type();
      const Expression & get_expression();
    private:
      ExpressionPostfixIncDec::OperationType type;
      JLang::owned<Terminal> operator_token;
      JLang::owned<Expression> expression;
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
                            JLang::owned<Terminal> _operator_token,
                            JLang::owned<Expression> _expression,
                            ExpressionUnaryPrefix::OperationType _type
                            );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionUnaryPrefix();
      const ExpressionUnaryPrefix::OperationType & get_type();
      const JLang::context::SourceReference & get_operator_source_ref() const;
      const Expression & get_expression();
    private:
      ExpressionUnaryPrefix::OperationType type;
      JLang::owned<Terminal> operator_token;
      JLang::owned<Expression> expression;
    };

    class ExpressionUnarySizeofType : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionUnarySizeofType(
                                JLang::owned<Terminal> _sizeof_token,
                                JLang::owned<Terminal> _paren_l_token,
                                JLang::owned<TypeSpecifier> _type_specifier,
                                JLang::owned<Terminal> _paren_r_token
                                );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionUnarySizeofType();
      const TypeSpecifier & get_type_specifier() const;
    private:
      JLang::owned<Terminal> sizeof_token;
      JLang::owned<Terminal> paren_l_token;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> paren_r_token;
    };

    class ExpressionCast : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionCast(
                     JLang::owned<Terminal> _cast_token,
                     JLang::owned<Terminal> _paren_l_token,
                     JLang::owned<TypeSpecifier> _type_specifier,
                     JLang::owned<Terminal> _comma_token,
                     JLang::owned<Expression> _expression,
                     JLang::owned<Terminal> _paren_r_token
                     );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionCast();
      const TypeSpecifier & get_type() const;
      const Expression & get_expression() const;
    private:
      JLang::owned<Terminal> cast_token;
      JLang::owned<Terminal> paren_l_token;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> comma_token;
      JLang::owned<Expression> expression;
      JLang::owned<Terminal> paren_r_token;
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
                       JLang::owned<Expression> _expression_a,
                       JLang::owned<Terminal> _operator_token,
                       JLang::owned<Expression> _expression_b,
                       ExpressionBinary::OperationType _type
                       );
      /**
       * This form of the constructor is introduced because
       * of a token oddity.  The && symbol can't be a single
       * token because that causes problems with declaring
       * references to references (i.e. int &&a -> int & & a)
       * so we need to only recognize & as a token.  Therefore
       * the grammar for a binary and is expresion ANDPERSAND ANDPERSAND expression
       * so even though it's a binary operation, it can take two
       * operator arguments.  This is purely a quirk of the
       * syntax and there are not new semantics because
       * of it.
       */
      ExpressionBinary(
                       JLang::owned<Expression> _expression_a,
                       JLang::owned<Terminal> _operator_token,
                       JLang::owned<Terminal> _operator_token2,
                       JLang::owned<Expression> _expression_b,
                       ExpressionBinary::OperationType _type
                       );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionBinary();
      const Expression & get_a() const;
      const ExpressionBinary::OperationType & get_operator() const;
      const JLang::context::SourceReference & get_operator_source_ref() const;
      const Expression & get_b() const;
      
    private:
      OperationType type;
      JLang::owned<Expression> expression_a;
      JLang::owned<Terminal> operator_token;
      JLang::owned<Terminal> operator_token2;
      JLang::owned<Expression> expression_b;
    };
    
    class ExpressionTrinary : public JLang::frontend::ast::SyntaxNode {
    public:
      ExpressionTrinary(
                        JLang::owned<Expression> _condition,
                        JLang::owned<Terminal> _questionmark_token,
                        JLang::owned<Expression> _if_expression,
                        JLang::owned<Terminal> _colon_token,
                        JLang::owned<Expression> _else_expression
                        );
    /**
     * Destructor, nothing special.
     */
      ~ExpressionTrinary();
      const Expression & get_condition() const;
      const Expression & get_if() const;
      const Expression & get_else() const;
    private:
      JLang::owned<Expression> condition;
      JLang::owned<Terminal> questionmark_token;
      JLang::owned<Expression> if_expression;
      JLang::owned<Terminal> colon_token;
      JLang::owned<Expression> else_expression;      
    };
    
    class Expression : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<JLang::owned<ExpressionPrimary>,
                           JLang::owned<ExpressionPostfixArrayIndex>,
                           JLang::owned<ExpressionPostfixFunctionCall>,
                           JLang::owned<ExpressionPostfixDot>,
                           JLang::owned<ExpressionPostfixArrow>,
                           JLang::owned<ExpressionPostfixIncDec>,
                           JLang::owned<ExpressionUnaryPrefix>,
                           JLang::owned<ExpressionUnarySizeofType>,
                           JLang::owned<ExpressionBinary>,
                           JLang::owned<ExpressionTrinary>,
                           JLang::owned<ExpressionCast>
                           > ExpressionType;
      Expression(Expression::ExpressionType _expression_type, const SyntaxNode & _sn);
    /**
     * Destructor, nothing special.
     */
      ~Expression();
      const Expression::ExpressionType & get_expression() const;
    private:
      Expression::ExpressionType expression_type;
    };
    
    class GlobalInitializerExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerExpressionPrimary(JLang::owned<Terminal> _equals_token,
                                         JLang::owned<ExpressionPrimary> _expression
                                         );
    /**
     * Destructor, nothing special.
     */
      ~GlobalInitializerExpressionPrimary();
      const ExpressionPrimary & get_expression() const;
    private:
      JLang::owned<Terminal> equals_token;
      JLang::owned<ExpressionPrimary> expression;

    };
    class GlobalInitializerAddressofExpressionPrimary : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerAddressofExpressionPrimary(
                                                  JLang::owned<Terminal> _equals_token,
                                                  JLang::owned<Terminal> _addressof_token,
                                                  JLang::owned<ExpressionPrimary> _expression
                                         );
    /**
     * Destructor, nothing special.
     */
      ~GlobalInitializerAddressofExpressionPrimary();
      const ExpressionPrimary & get_expression() const;
    private:
      JLang::owned<Terminal> equals_token;
      JLang::owned<Terminal> addressof_token;
      JLang::owned<ExpressionPrimary> expression;
    };

    class StructInitializer : public JLang::frontend::ast::SyntaxNode {
    public:
      StructInitializer(
                        JLang::owned<Terminal> _dot_token,
                        JLang::owned<Terminal> _identifier_token,
                        JLang::owned<GlobalInitializer> _global_initializer,
                        JLang::owned<Terminal> _semicolon_token
                        );
    /**
     * Destructor, nothing special.
     */
      ~StructInitializer();
      const GlobalInitializer & get_initializer() const;
    private:
      JLang::owned<Terminal> dot_token;
      JLang::owned<Terminal> identifier_token;
      JLang::owned<GlobalInitializer> global_initializer;
      JLang::owned<Terminal> semicolon_token;
    };
          
    class StructInitializerList : public JLang::frontend::ast::SyntaxNode {
    public:
      StructInitializerList(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
      ~StructInitializerList();
      void add_initializer(JLang::owned<StructInitializer> initializer);
      const std::vector<JLang::owned<StructInitializer>> & get_initializers() const;
    private:
      std::vector<JLang::owned<StructInitializer>> initializers;
    };
          
    class GlobalInitializerStructInitializerList : public JLang::frontend::ast::SyntaxNode {
    public:
      GlobalInitializerStructInitializerList(
                                             JLang::owned<Terminal> _equals_token,
                                             JLang::owned<Terminal> _brace_l_token,
                                             JLang::owned<StructInitializerList> _struct_initializer,
                                             JLang::owned<Terminal> _brace_r_token
                                             );

    /**
     * Destructor, nothing special.
     */
      ~GlobalInitializerStructInitializerList();
      const StructInitializerList & get_struct_initializer() const;
    private:
      JLang::owned<Terminal> equals_token;
      JLang::owned<Terminal> brace_l_token;
      JLang::owned<StructInitializerList> struct_initializer;
      JLang::owned<Terminal> brace_r_token;
    };
    
    class GlobalInitializer : public JLang::frontend::ast::SyntaxNode {
    public:
      typedef std::variant<
        JLang::owned<GlobalInitializerExpressionPrimary>,
        JLang::owned<GlobalInitializerAddressofExpressionPrimary>,
        JLang::owned<GlobalInitializerStructInitializerList>,
        nullptr_t> GlobalInitializerType;
      GlobalInitializer(const JLang::context::SourceReference & _source_ref);
      GlobalInitializer(GlobalInitializerType initializer, const JLang::frontend::ast::SyntaxNode & _sn);
    /**
     * Destructor, nothing special.
     */
      ~GlobalInitializer();
      const GlobalInitializerType & get_initializer() const;
    private:
      GlobalInitializerType initializer;
    };
    
    class FileStatementGlobalDefinition : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementGlobalDefinition(
                                    JLang::owned<AccessModifier> _access_modifier,
                                    JLang::owned<UnsafeModifier> _unsafe_modifier,
                                    JLang::owned<TypeSpecifier> _type_specifier,
                                    JLang::owned<Terminal> _name,
                                    JLang::owned<ArrayLength> _array_length,
                                    JLang::owned<GlobalInitializer> _global_initializer,
                                    JLang::owned<Terminal> _semicolon
                                    );
    /**
     * Destructor, nothing special.
     */
      ~FileStatementGlobalDefinition();
      const AccessModifier & get_access_modifier() const;
      const UnsafeModifier & get_unsafe_modifier() const;
      const TypeSpecifier & get_type_specifier() const;
      const std::string & get_name() const;
      const JLang::context::SourceReference & get_name_source_ref() const;
      const ArrayLength & get_array_length() const;
      const GlobalInitializer & get_global_initializer() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<UnsafeModifier> unsafe_modifier;
      JLang::owned<TypeSpecifier> type_specifier;
      JLang::owned<Terminal> name; // function name (IDENTIFIER)
      JLang::owned<ArrayLength> array_length;
      JLang::owned<GlobalInitializer> global_initializer;
      JLang::owned<Terminal> semicolon;
    };
    
    class NamespaceDeclaration : public JLang::frontend::ast::SyntaxNode {
    public:
      NamespaceDeclaration(
                           JLang::owned<AccessModifier> _access_modifier,
                           JLang::owned<Terminal> _namespace_token,
                           JLang::owned<Terminal> _identifier_token
                           );
    /**
     * Destructor, nothing special.
     */
      ~NamespaceDeclaration();
      const AccessModifier & get_access_modifier() const;
      const Terminal & get_name() const;
    private:
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<Terminal> namespace_token;
      JLang::owned<Terminal> identifier_token;
    };

    class FileStatementNamespace : public JLang::frontend::ast::SyntaxNode {
    public:
      FileStatementNamespace(JLang::owned<NamespaceDeclaration> _namespace_declaration,
                             JLang::owned<Terminal> _brace_l_token,
                             JLang::owned<FileStatementList> _file_statement_list,
                             JLang::owned<Terminal> _brace_r_token,
                             JLang::owned<Terminal> _semicolon_token
                             );
    /**
     * Destructor, nothing special.
     */
      ~FileStatementNamespace();
      const NamespaceDeclaration & get_declaration() const;
      const FileStatementList & get_statement_list() const;
    private:
      JLang::owned<NamespaceDeclaration> namespace_declaration;
      JLang::owned<Terminal> brace_l_token;
      JLang::owned<FileStatementList> file_statement_list;
      JLang::owned<Terminal> brace_r_token;
      JLang::owned<Terminal> semicolon_token;
    };

    class UsingAs : public JLang::frontend::ast::SyntaxNode {
    public:
      UsingAs(
              JLang::owned<Terminal> _as_token,
              JLang::owned<Terminal> _identifier_token
              );
      UsingAs(const JLang::context::SourceReference & _source_ref);
    /**
     * Destructor, nothing special.
     */
      ~UsingAs();
      bool is_as() const;
      const std::string & get_using_name() const;
      const JLang::context::SourceReference & get_using_name_source_ref() const;
    private:
      bool aas;
      std::string using_name;
      JLang::owned<Terminal> as_token;
      JLang::owned<Terminal> identifier_token;
    };
    
    class FileStatementUsing : public JLang::frontend::ast::SyntaxNode {
    public:
          FileStatementUsing(JLang::owned<AccessModifier> _access_modifier,
                             JLang::owned<Terminal> _using,
                             JLang::owned<Terminal> _namespace,
                             JLang::owned<Terminal> _namespace_name,
                             JLang::owned<UsingAs> _using_as,
                             JLang::owned<Terminal> _semicolon);
    /**
     * Destructor, nothing special.
     */
      ~FileStatementUsing();
    private:
      const AccessModifier & get_access_modifier() const;
      const std::string & get_namespace() const;
      const JLang::context::SourceReference & get_namespace_source_ref() const;
      const UsingAs &get_using_as() const;
      
      JLang::owned<AccessModifier> access_modifier;
      JLang::owned<Terminal> using_token;
      JLang::owned<Terminal> namespace_token;
      JLang::owned<Terminal> namespace_name_token;
      JLang::owned<UsingAs> using_as;
      JLang::owned<Terminal> semicolon_token;
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
      FileStatementList(const JLang::context::SourceReference & _source_ref);
      FileStatementList(JLang::owned<Terminal> _yyeof);
    /**
     * Destructor, nothing special.
     */
      ~FileStatementList();
      const std::vector<JLang::owned<FileStatement>> & get_statements() const;
      void add_statement(JLang::owned<FileStatement> statement);
    private:
      JLang::owned<Terminal> yyeof;
      std::vector<JLang::owned<FileStatement>> statements;
    };

  //! Represents the top-level syntax unit for a source file.
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
                      JLang::owned<FileStatementList> file_statement_list,
                      JLang::owned<Terminal> yyeof_token
                      );
    /**
     * Destructor, nothing special.
     */
      ~TranslationUnit();
      /**
       * This method returns a list of 'const' owned pointers to the
       * statements defined in this translation unit.
       */
      const std::vector<JLang::owned<FileStatement>> & get_statements() const;
    private:
      JLang::owned<Terminal> yyeof_token;
      JLang::owned<FileStatementList> file_statement_list;
      
    };
    
};

/*! @} End of Doxygen Groups*/
