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
#ifndef _GYOJI_INTERNAL
#error "This header is intended to be used internally as a part of the Gyoji front-end.  Please include frontend.hpp instead."
#endif
#pragma once


/*!
 *  \addtogroup Frontend
 *  @{
 */

/**
 * @brief Strongly-typed syntax tree.
 * @details
 * Strongly-typed representation of the parse tree
 * resulting from reading and parsing an input file.
 */
namespace Gyoji::frontend::tree {
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
	TerminalNonSyntax(Type _type, const Gyoji::context::Token & _token);
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
	const Gyoji::context::Token & token;
    };

    //! Represents tokens from the lexer used to represent keywords and identifiers found in the source.
    /**
     * Terminals are the raw tokens received by the lexer.
     * They carry the token information as well as any
     * "Non-syntax" data like comments and whitespace.
     */
    class Terminal : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * This is the type for identifiers.  Identifiers
	 * may be global symbols like functions or global/static
	 * variables.  In this case, they must be resolved by
	 * the parser because resolution depends on the namespace
	 * context which is available only during parsing.  This is a quirk
	 * of the C/C++ syntax.  Identifiers may also be "local" identifiers
	 * like member variables or local variables which exist in a scope
	 * other than that global/namespace scope.  In the 'local' case, resolution
	 * of these is deferred to the MIR layer, so we may end up passing
	 * invalid tokens to the MIR where the resolution may fail at that point
	 * or somewhere in the analysis phase.
	 */
	typedef enum {
	    /**
	     * Indicates that this identifier is something in a 'global'
	     * scope, for example, in the root namespace or in a sub-namespace.
	     * For example, the variable 'g' in 'namespace { u32 g; }' would be
	     * an identifier in global scope.
	     */
	    IDENTIFIER_GLOBAL_SCOPE,
	    /**
	     * Indicates that the identifier is something in a 'local' scope
	     * such as an argument of a function, a local variable in a scope,
	     * or a class member.
	     */
	    IDENTIFIER_LOCAL_SCOPE,

	    /**
	     * This is an uncategorized identifier.  It is not in local scope
	     * nor is it in global scope.  For example, this may be the name
	     * of a namespace or other thing that is consumed only at the
	     * syntax level and has no semantic meaning.
	     */
	    IDENTIFIER_UNCATEGORIZED
	} IdentifierType;
    
	/**
	 * Construct a terminal from the corresponding
	 * lexer token.
	 */
	Terminal(const Gyoji::context::Token & _token);
	/**
	 * Destructor, nothing special.
	 */
	~Terminal();
	
	/**
	 * Returns the type of the correspinding
	 * lexer token.
	 */
	const Gyoji::context::TokenID & get_type() const;
	/**
	 * Returns the matched data from the input
	 * that matched the token.
	 */
	const std::string & get_value() const;
	
	/**
	 * Returns a reference to the place in the source-file
	 * where this terminal occurred.
	 */
	const Gyoji::context::SourceReference & get_terminal_source_ref() const;
	
	/**
	 * For the case of identifier tokens, this returns
	 * the fully-qualified name of the type, namespace,
	 * or identifier once namespace resolution has
	 * identified it.
	 */
	std::string get_fully_qualified_name() const;
	
	/**
	 * Return the 'leaf node' name.
	 */
	std::string get_name() const;

	const IdentifierType & get_identifier_type() const;
	void set_identifier_type(IdentifierType _identifier_type);
	
	// The terminal "owns" uniquely all of the non-syntax data
	// in this vector.  It may be returned to access it,
	// but these are owned pointers, so they must only
	// be de-referenced and never assigned to
	std::vector<Gyoji::owned<TerminalNonSyntax>> non_syntax;

	void set_ns2_entity(Gyoji::frontend::namespaces::NS2Entity *_ns_entity);
	Gyoji::frontend::namespaces::NS2Entity *get_ns2_entity() const;
	
	
    private:
	const Gyoji::context::Token & token;
	std::string fully_qualified_name;
	IdentifierType identifier_type;
	// Tells us whether this is a namespace name, an entity, or what.
	Gyoji::frontend::namespaces::NS2Entity *ns2_entity;
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
    class FileStatement : public Gyoji::frontend::ast::SyntaxNode {
    public:
	typedef std::variant<
            Gyoji::owned<FileStatementFunctionDefinition>,
            Gyoji::owned<FileStatementFunctionDeclaration>,
            Gyoji::owned<FileStatementGlobalDefinition>,
	    Gyoji::owned<ClassDeclaration>,
            Gyoji::owned<ClassDefinition>,
            Gyoji::owned<EnumDefinition>,
            Gyoji::owned<TypeDefinition>,
            Gyoji::owned<FileStatementNamespace>,
            Gyoji::owned<FileStatementUsing>> FileStatementType;
    
	FileStatement(FileStatementType _statement, const Gyoji::frontend::ast::SyntaxNode & _sn);
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
	 *       const auto & function_def = std::get<Gyoji::owned<FileStatementFunctionDefinition>>(stmt);
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
    class AccessQualifier : public Gyoji::frontend::ast::SyntaxNode {
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
	AccessQualifier(Gyoji::owned<Terminal> _qualifier);
	/**
	 * Constructs a default access qualifier (UNSPECIFIED)
	 * if no qualifier is specified in the source file.
	 */
	AccessQualifier(const Gyoji::context::SourceReference & _source_ref);
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
	Gyoji::owned<Terminal> qualifier;
    };
    
    //! Declares PUBLIC, PROTECTED, or PRIVATE access to functions and members.
    /**
     * An access modifier is a declaration of the visibility of
     * an object.  If it is not specified, it is presumed to
     * be public.  If it is specified, then the protection will
     * be evaluated by the namespace system according to where
     * it is referenced with respect to where it is declared.
     */
    class AccessModifier : public Gyoji::frontend::ast::SyntaxNode {
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
	AccessModifier(Gyoji::owned<Terminal> _modifier);
	/**
	 * This is the default constructor used when the
	 * access modifier is not explicitly specified in the
	 * source file (i.e. optional).
	 */
	AccessModifier(const Gyoji::context::SourceReference & _source_ref);
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
	Gyoji::owned<Terminal> modifier;
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
    class UnsafeModifier : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Constructs an unsafe modifier from the parse token
	 * in the input.  This indicates that a block HAS been marked
	 * as unsafe.
	 */
	UnsafeModifier(Gyoji::owned<Terminal> _unsafe_token);
	/**
	 * The default unsafe modifier (i.e. not specified) refers
	 * to a block that is NOT marked as unsafe.
	 */
	UnsafeModifier(const Gyoji::context::SourceReference & _source_ref);
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
	Gyoji::owned<Terminal> unsafe_token;
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
    class TypeName : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Constructs a simple type name representing the
	 * type specified in the source file.
	 */
	TypeName(Gyoji::owned<Terminal> _type_name);
	
	/**
	 * Constructs a 'typeof' expression where the type
	 * has the meaning of the type of the expression
	 * specified.
	 */
	TypeName(Gyoji::owned<Terminal> _typeof_token,
		 Gyoji::owned<Terminal> _paren_l_token,
		 Gyoji::owned<Expression> _expression,
		 Gyoji::owned<Terminal> _paren_r_token
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
	std::string get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	/**
	 * Returns the expression representing the type
	 * being referenced.  Only safe to call when is_expression()
	 * returns true.
	 */
	const Expression & get_expression() const;
    private:
	bool m_is_expression;
	// For raw names
	Gyoji::owned<Terminal> type_name;
	// for typeof expressions
	Gyoji::owned<Terminal> typeof_token;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> paren_r_token;
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
    class TypeSpecifierCallArgs : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Constructs a new empty template argument list.
	 */
	TypeSpecifierCallArgs(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~TypeSpecifierCallArgs();
	/**
	 * This adds the given type specifier to the list of
	 * template arguments.
	 */
	void add_argument(Gyoji::owned<TypeSpecifier> _argument);
	/**
	 * This adds the next argument to the list along with accounting for
	 * the comma that separates the arguments.  The comma token is
	 * consumed here and is not useful for the semantics, but must be
	 * consumed so that the syntax tree can accurately reproduce the output.
	 */
	void add_argument(Gyoji::owned<Terminal> _comma_token, Gyoji::owned<TypeSpecifier> _argument);
	
	/**
	 * This provides immutable access to the list of arguments.
	 */
	const std::vector<Gyoji::owned<TypeSpecifier>> & get_arguments() const;
    private:
	std::vector<Gyoji::owned<Terminal>> comma_list;
	std::vector<Gyoji::owned<TypeSpecifier>> arguments;
    };
    
    //! Represents a simple type.
    /**
     * This type specifies a type along with the access qualifier
     * for the type.  For example, "const int" or "volatile float"
     * would be expressions that resolve as a type specifier.
     */
    class TypeSpecifierSimple : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Constructs a type qualified by its access semantics.
	 */
	TypeSpecifierSimple(
	    Gyoji::owned<AccessQualifier> _access_qualifier,
	    Gyoji::owned<TypeName> _type_name
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
	Gyoji::owned<AccessQualifier> access_qualifier;
	Gyoji::owned<TypeName> type_name;
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
    class TypeSpecifierTemplate : public Gyoji::frontend::ast::SyntaxNode {
    public:
	TypeSpecifierTemplate(
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<TypeSpecifierCallArgs> _type_specifier_call_args,
	    Gyoji::owned<Terminal> _paren_r_token
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
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<TypeSpecifierCallArgs> type_specifier_call_args;
	Gyoji::owned<Terminal> paren_r_token;
    };
    
    //! Represents a pointer to a function.
    /**
     * This type represents a pointer to a function.  This is designed to follow
     * the c-style syntax of function pointers.  This isn't the most efficient
     * or readable way to specify function pointers, but is is <em>traditional</em>
     * and well-known to those familiar with the C syntax.
     */
    class TypeSpecifierFunctionPointer : public Gyoji::frontend::ast::SyntaxNode {
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
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _paren_l1_token,
	    Gyoji::owned<Terminal> _star_token,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _paren_r1_token,
	    Gyoji::owned<Terminal> _paren_l2_token,
	    Gyoji::owned<FunctionDefinitionArgList> _function_definition_arg_list,
	    Gyoji::owned<Terminal> _paren_r2_token
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
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	/**
	 * Returns the list of arguments the function expects.
	 */
	const FunctionDefinitionArgList & get_args() const;
    private:
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> paren_l1_token;
	Gyoji::owned<Terminal> star_token;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> paren_r1_token;
	Gyoji::owned<Terminal> paren_l2_token;
	Gyoji::owned<FunctionDefinitionArgList> function_definition_arg_list;
	Gyoji::owned<Terminal> paren_r2_token;
    };
    
    /**
     * @brief Represents an unsafe poniter to a specific type.
     *
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
    class TypeSpecifierPointerTo : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Specifies a pointer to the given type and
	 * marked with access as 'const', 'volatile', or 'unspecified'.
	 */
	TypeSpecifierPointerTo(
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _star_token,
	    Gyoji::owned<AccessQualifier> _access_qualifier
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
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> star_token;
	Gyoji::owned<AccessQualifier> access_qualifier;
    };

    class TypeSpecifierArray : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Specifies a pointer to the given type and
	 * marked with access as 'const', 'volatile', or 'unspecified'.
	 */
	TypeSpecifierArray(
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _bracket_l_token,
	    Gyoji::owned<Terminal> _literal_int_token,
	    Gyoji::owned<Terminal> _bracket_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~TypeSpecifierArray();
	
	/**
	 * Returns the type that is accessed behind this pointer.
	 */
	const TypeSpecifier & get_type_specifier() const;

	/**
	 * Returns the literal integer used to specify the size
	 * of the array.
	 */
	const Terminal & get_literal_int_token() const;
    private:
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> bracket_l_token;
	Gyoji::owned<Terminal> literal_int_token;
	Gyoji::owned<Terminal> bracket_r_token;
    };

    /**
     * @brief Represents a safe reference to a specific type.
     *
     * @details
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
    class TypeSpecifierReferenceTo : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Constructs a reference to the given type
	 * using the access qualifiers given.
	 */
	TypeSpecifierReferenceTo(
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _andpersand_token,
	    Gyoji::owned<AccessQualifier> _access_qualifier
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
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> andpersand_token;
	Gyoji::owned<AccessQualifier> access_qualifier;
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
     *      if (std::holds_alternative<Gyoji::owned<TypeSpecifierSimple>>(type)) {
     *          const Gyoji::owned<TypeSpecifierSimple> & simple = std::get<Gyoji::owned<TypeSpecifierSimple>>(type);
     *          std::string name = simple.get_name();
     *      }
     * </pre>
     */
    class TypeSpecifier : public Gyoji::frontend::ast::SyntaxNode {
    public:
	typedef std::variant<
	    Gyoji::owned<TypeSpecifierSimple>,
	    Gyoji::owned<TypeSpecifierTemplate>,
	    Gyoji::owned<TypeSpecifierFunctionPointer>,
	    Gyoji::owned<TypeSpecifierPointerTo>,
            Gyoji::owned<TypeSpecifierReferenceTo>,
	    Gyoji::owned<TypeSpecifierArray>
	> TypeSpecifierType;
	TypeSpecifier(TypeSpecifier::TypeSpecifierType _type, const Gyoji::frontend::ast::SyntaxNode & _sn);
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
    class FunctionDefinitionArg : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FunctionDefinitionArg(Gyoji::owned<TypeSpecifier> _type_specifier,
			      Gyoji::owned<Terminal> _identifier_token
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
	const Terminal & get_identifier() const;
    private:
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> identifier_token;
    };
    
    //! Represents the list of arguments to a function definition.
    /**
     * This represents a list of argumnents to a function.  The input syntax
     * consists of a comma-separated list of arguments of type FunctionDefinitionArg.
     */
    class FunctionDefinitionArgList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FunctionDefinitionArgList(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~FunctionDefinitionArgList();
	/**
	 * Returns the list of arguments to the function.
	 */
	const std::vector<Gyoji::owned<FunctionDefinitionArg>> & get_arguments() const;
	/**
	 * Adds a single argument to the function, used during
	 * the parse stage to construct the function arguments
	 * from the input syntax.
	 */
	void add_argument(Gyoji::owned<FunctionDefinitionArg> _argument);
	/**
	 * Adds the comma separator to the function arguments, but this
	 * plays no role in the semantics.
	 */
	void add_comma(Gyoji::owned<Terminal> _comma);
    private:
	std::vector<Gyoji::owned<Terminal>> commas;
	std::vector<Gyoji::owned<FunctionDefinitionArg>> arguments;
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
    class FileStatementFunctionDeclaration : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FileStatementFunctionDeclaration(
	    Gyoji::owned<FileStatementFunctionDeclStart> _start,
	    Gyoji::owned<Terminal> _paren_l,
	    Gyoji::owned<FunctionDefinitionArgList> _arguments,
	    Gyoji::owned<Terminal> _paren_r,
	    Gyoji::owned<Terminal> _semicolon
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
	const Terminal & get_name() const;
	/**
	 * Specifies the list of arguments with names
	 * and types for the function.
	 */
	const FunctionDefinitionArgList & get_arguments() const;
	
    private:
	Gyoji::owned<FileStatementFunctionDeclStart> start; 
	Gyoji::owned<Terminal> paren_l; // argument list delimiter PAREN_L
	Gyoji::owned<FunctionDefinitionArgList> arguments;
	Gyoji::owned<Terminal> paren_r; // argument list delimiter PAREN_R
	Gyoji::owned<Terminal> semicolon; // argument list delimiter SEMICOLON
    };
    
    class InitializerExpression : public Gyoji::frontend::ast::SyntaxNode {
    public:
	InitializerExpression(
	    const Gyoji::context::SourceReference & _src_ref
	    );
	InitializerExpression(
	    Gyoji::owned<Terminal> _equals_token,
	    Gyoji::owned<Expression> _expression
	    );
	~InitializerExpression();
	bool has_expression() const;
	const Expression & get_expression() const;
    private:
	Gyoji::owned<Terminal> equals_token;
	Gyoji::owned<Expression> expression;
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
    class StatementVariableDeclaration : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Used to create a variable declaration during the parse stage,
	 * consuming the parse tokens.
	 */
	StatementVariableDeclaration(
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<InitializerExpression> _initializer,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	StatementVariableDeclaration(
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<ArgumentExpressionList> _argument_expression_list,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<Terminal> _semicolon_token
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
	const Terminal & get_identifier() const;

	bool is_constructor() const;
	
	/**
	 * Returns the initializer expression, usually a literal,
	 * that is used to populate the variable with known values.
	 * Only valid if !is_constructor().
	 */
	const InitializerExpression & get_initializer_expression() const;

	/**
	 * Returns the list of constructor arguments to pass.
	 * Only valid if is_constructor().
	 */
	const ArgumentExpressionList & get_argument_expression_list() const;

    private:
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<ArrayLength> array_length;

	bool m_is_constructor;
	Gyoji::owned<InitializerExpression> initializer;

	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<ArgumentExpressionList> argument_expression_list;
	Gyoji::owned<Terminal> paren_r_token;
	
	Gyoji::owned<Terminal> semicolon_token;
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
    class StatementBlock : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Used in the parse stage to create a block of statements
	 * with an optional unsafe modifier.
	 */
	StatementBlock(
	    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
	    Gyoji::owned<ScopeBody> _scope_body
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
	Gyoji::owned<UnsafeModifier> unsafe_modifier;
	Gyoji::owned<ScopeBody> scope_body;
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
    class StatementExpression : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementExpression(
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StatementExpression();
	const Expression & get_expression() const;
    private:
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    //! This represents an if condition in one of several forms.
    /**
     * The if/else construct in Gyoji comes in several forms.  It is
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
    class StatementIfElse : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Constructs an 'if' statement with a single block
	 * for when the condition is true.
	 */
	StatementIfElse(
	    Gyoji::owned<Terminal> _if_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<ScopeBody> _if_scope_body
	    );
	/**
	 * Constructs an if/else block where one block
	 * represents the expression true and the other
	 * block represents the code for the expression false.
	 */
	StatementIfElse(
	    Gyoji::owned<Terminal> _if_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<ScopeBody> _if_scope_body,
	    Gyoji::owned<Terminal> _else_token,
	    Gyoji::owned<ScopeBody> _else_scope_body
	    );
	/**
	 * Constructs an if/elseif chain where each 'else if'
	 * can be modelled as the next element of a chain of
	 * conditions.
	 */
	StatementIfElse(
	    Gyoji::owned<Terminal> _if_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<ScopeBody> _if_scope_body,
	    Gyoji::owned<Terminal> _else_token,
	    Gyoji::owned<StatementIfElse> _statement_if_else
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
	Gyoji::owned<Terminal> if_token;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> paren_r_token;
	Gyoji::owned<ScopeBody> if_scope_body;
	Gyoji::owned<Terminal> else_token;
	Gyoji::owned<ScopeBody> else_scope_body;
	Gyoji::owned<StatementIfElse> else_if;
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
    class StatementWhile : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementWhile(
	    Gyoji::owned<Terminal> _while_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<ScopeBody> _scope_body
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
	Gyoji::owned<Terminal> while_token;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> paren_r_token;
	Gyoji::owned<ScopeBody> scope_body;
    };
    
    class StatementFor : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementFor(
	    Gyoji::owned<Terminal> _for_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<Expression> _expression_initial,
	    Gyoji::owned<Terminal> _semicolon_initial,
	    Gyoji::owned<Expression> _expression_termination,
	    Gyoji::owned<Terminal> _semicolon_termination,
	    Gyoji::owned<Expression> _expression_increment,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<ScopeBody> _scope_body
	    );
	StatementFor(
	    Gyoji::owned<Terminal> _for_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _assignment_token,
	    Gyoji::owned<Expression> _expression_initial,
	    Gyoji::owned<Terminal> _semicolon_initial,
	    Gyoji::owned<Expression> _expression_termination,
	    Gyoji::owned<Terminal> _semicolon_termination,
	    Gyoji::owned<Expression> _expression_increment,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<ScopeBody> _scope_body
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StatementFor();
	bool is_declaration() const;

	/**
	 * Only valid if is_declaration() returns true.
	 */
	const TypeSpecifier & get_type_specifier() const;

	/**
	 * Only valid if is_declaration() returns true.
	 */
	const Terminal & get_identifier() const;
	
	const Expression & get_expression_initial() const;
	const Expression & get_expression_termination() const;
	const Expression & get_expression_increment() const;
	const ScopeBody & get_scope_body() const;
    private:
	bool is_declaration_initializer;
	Gyoji::owned<Terminal> for_token;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> assignment_token;
	Gyoji::owned<Expression> expression_initial;
	Gyoji::owned<Terminal> semicolon_initial;
	Gyoji::owned<Expression> expression_termination;
	Gyoji::owned<Terminal> semicolon_termination;
	Gyoji::owned<Expression> expression_increment;
	Gyoji::owned<Terminal> paren_r_token;
	Gyoji::owned<ScopeBody> scope_body;
    };
    
    class StatementSwitchBlock : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementSwitchBlock(
	    Gyoji::owned<Terminal> _default_token,
	    Gyoji::owned<Terminal> _colon_token,
	    Gyoji::owned<ScopeBody> _scope_body
	    );
	StatementSwitchBlock(
	    Gyoji::owned<Terminal> _case_token,
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _colon_token,
	    Gyoji::owned<ScopeBody> _scope_body
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
	Gyoji::owned<Terminal> default_token;
	// For the expression case
	Gyoji::owned<Terminal> case_token;
	Gyoji::owned<Expression> expression;
	// Common:
	Gyoji::owned<Terminal> colon_token;
	Gyoji::owned<ScopeBody> scope_body;
    };
    
    class StatementSwitchContent : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementSwitchContent(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~StatementSwitchContent();
	const std::vector<Gyoji::owned<StatementSwitchBlock>> & get_blocks() const;
	void add_block(Gyoji::owned<StatementSwitchBlock> _block);
    private:
	std::vector<Gyoji::owned<StatementSwitchBlock>> blocks;
    };
    
    class StatementSwitch : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementSwitch(
	    Gyoji::owned<Terminal> _switch_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<Expression> expression,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<Terminal> _brace_l_token,
	    Gyoji::owned<StatementSwitchContent> _switch_content,
	    Gyoji::owned<Terminal> _brace_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StatementSwitch();
	const Expression & get_expression() const;
	const StatementSwitchContent & get_switch_content() const;
    private:
	Gyoji::owned<Terminal> switch_token;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> paren_r_token;
	Gyoji::owned<Terminal> brace_l_token;
	Gyoji::owned<StatementSwitchContent> switch_content;
	Gyoji::owned<Terminal> brace_r_token;
    };
    class StatementLabel : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementLabel(
	    Gyoji::owned<Terminal> _label_token,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _colon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StatementLabel();
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
    private:
	Gyoji::owned<Terminal> label_token;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> colon_token;
    };
    class StatementGoto : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementGoto(
	    Gyoji::owned<Terminal> _goto_token,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StatementGoto();
	const std::string & get_label() const;
	const Gyoji::context::SourceReference & get_label_source_ref() const;
    private:
	Gyoji::owned<Terminal> goto_token;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    class StatementBreak : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementBreak(
	    Gyoji::owned<Terminal> _break_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StatementBreak();
    private:
	Gyoji::owned<Terminal> break_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    class StatementContinue : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementContinue(
	    Gyoji::owned<Terminal> _continue_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StatementContinue();
    private:
	Gyoji::owned<Terminal> continue_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    class StatementReturn : public Gyoji::frontend::ast::SyntaxNode {
    public:
	/**
	 * Constructs a return statement with an expression.
	 */
	StatementReturn(
	    Gyoji::owned<Terminal> _return_token,
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Constructs a return statement for 'void' functions.
	 */
	StatementReturn(
	    Gyoji::owned<Terminal> _return_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StatementReturn();
	bool is_void() const;
	const Expression & get_expression() const;
    private:
	Gyoji::owned<Terminal> return_token;
	bool void_return;
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    class Statement : public Gyoji::frontend::ast::SyntaxNode {
    public:
	typedef std::variant<
	    Gyoji::owned<StatementVariableDeclaration>,
	    Gyoji::owned<StatementBlock>,
	    Gyoji::owned<StatementExpression>,
	    Gyoji::owned<StatementIfElse>,
	    Gyoji::owned<StatementWhile>,
	    Gyoji::owned<StatementFor>,
	    Gyoji::owned<StatementSwitch>,
	    Gyoji::owned<StatementLabel>,
	    Gyoji::owned<StatementGoto>,
	    Gyoji::owned<StatementBreak>,
	    Gyoji::owned<StatementContinue>,
	    Gyoji::owned<StatementReturn>
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
    
    class StatementList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StatementList(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~StatementList();
	void add_statement(Gyoji::owned<Statement> _statement);
	const std::vector<Gyoji::owned<Statement>> &get_statements() const;
    private:
	std::vector<Gyoji::owned<Statement>> statements;
    };
    
    class ScopeBody : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ScopeBody(
	    Gyoji::owned<Terminal> brace_l_token,
	    Gyoji::owned<StatementList> statement_list,
	    Gyoji::owned<Terminal> brace_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ScopeBody();
	const StatementList & get_statements() const;
	const Gyoji::context::SourceReference & get_end_source_ref() const;
    private:
	Gyoji::owned<Terminal> brace_l_token;
	Gyoji::owned<StatementList> statement_list;
	Gyoji::owned<Terminal> brace_r_token;
    };

    class FileStatementFunctionDeclStart : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FileStatementFunctionDeclStart(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _name
	    );
	~FileStatementFunctionDeclStart();

	const AccessModifier & get_access_modifier() const;
	const UnsafeModifier & get_unsafe_modifier() const;
	const TypeSpecifier & get_type_specifier() const;
	const Terminal & get_name() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<UnsafeModifier> unsafe_modifier;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> name; // function name (IDENTIFIER)
    };
    
    class FileStatementFunctionDefinition : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FileStatementFunctionDefinition(
	    Gyoji::owned<FileStatementFunctionDeclStart> _start,
	    Gyoji::owned<Terminal> _paren_l,
	    Gyoji::owned<FunctionDefinitionArgList> _arguments,
	    Gyoji::owned<Terminal> _paren_r,
	    Gyoji::owned<ScopeBody> _scope_body
	    );
	/**
	 * Destructor, nothing special.
	 */
	~FileStatementFunctionDefinition();
	const AccessModifier & get_access_modifier() const;
	const UnsafeModifier & get_unsafe_modifier() const;
	const TypeSpecifier & get_return_type() const;
	const Terminal & get_name() const;
	const FunctionDefinitionArgList & get_arguments() const;
	const ScopeBody & get_scope_body() const;
	
    private:
	Gyoji::owned<FileStatementFunctionDeclStart> start;
	Gyoji::owned<Terminal> paren_l; // argument list delimiter PAREN_L
	Gyoji::owned<FunctionDefinitionArgList> arguments;
	Gyoji::owned<Terminal> paren_r; // argument list delimiter PAREN_R
	Gyoji::owned<ScopeBody> scope_body; // argument list delimiter SEMICOLON
    };
    
    class ArrayLength : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ArrayLength(const Gyoji::context::SourceReference & _source_ref);
	ArrayLength(
	    Gyoji::owned<Terminal> _bracket_l_token,
	    Gyoji::owned<Terminal> _literal_int_token,
	    Gyoji::owned<Terminal> _bracket_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ArrayLength();
	bool is_array() const;
	size_t get_size() const;
	const Gyoji::context::SourceReference & get_size_source_ref() const;
    private:
	Gyoji::owned<Terminal> bracket_l_token;
	Gyoji::owned<Terminal> literal_int_token;
	Gyoji::owned<Terminal> bracket_r_token;
    };
    
    class ClassDeclStart : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassDeclStart(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<Terminal> _class_token,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<ClassArgumentList> _class_argument_list,
	    bool is_identifier
	    );
	
	/**
	 * Destructor, nothing special.
	 */
	~ClassDeclStart();
	const AccessModifier & get_access_modifier() const;
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	
	const ClassArgumentList & get_argument_list() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<Terminal> class_token;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<ClassArgumentList> class_argument_list;
	std::string name;
    };
    class ClassArgumentList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassArgumentList(const Gyoji::context::SourceReference & _source_ref);
	ClassArgumentList(Gyoji::owned<Terminal> _argument);
	/**
	 * Destructor, nothing special.
	 */
	~ClassArgumentList();
	void add_argument(Gyoji::owned<Terminal> _comma, Gyoji::owned<Terminal> _argument);
	void add_parens(Gyoji::owned<Terminal> _paren_l, Gyoji::owned<Terminal> _paren_r);
	const std::vector<Gyoji::owned<Terminal>> & get_arguments() const;
    private:
	Gyoji::owned<Terminal> paren_l;
	std::vector<Gyoji::owned<Terminal>> comma_list;
	std::vector<Gyoji::owned<Terminal>> argument_list;
	Gyoji::owned<Terminal> paren_r;
    };
    
    class ClassMemberDeclarationVariable : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassMemberDeclarationVariable(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ClassMemberDeclarationVariable();
	const AccessModifier & get_access_modifier() const;
	const UnsafeModifier & get_unsafe_modifier() const;
	const TypeSpecifier & get_type_specifier() const;
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<UnsafeModifier> unsafe_modifier;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> semicolon_token;
    };    
    class ClassMemberDeclarationMethod : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassMemberDeclarationMethod(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<FunctionDefinitionArgList> _function_definition_arg_list,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ClassMemberDeclarationMethod();
	const AccessModifier & get_access_modifier() const;
	const UnsafeModifier & get_unsafe_modifier() const;
	const TypeSpecifier & get_type_specifier() const;
	const Terminal & get_identifier() const;
	const FunctionDefinitionArgList & get_arguments() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<UnsafeModifier> unsafe_modifier;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<FunctionDefinitionArgList> function_definition_arg_list;
	Gyoji::owned<Terminal> paren_r_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    class ClassMemberDeclarationConstructor : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassMemberDeclarationConstructor(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<FunctionDefinitionArgList> _function_definition_arg_list,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ClassMemberDeclarationConstructor();
	const AccessModifier & get_access_modifier() const;
	const UnsafeModifier & get_unsafe_modifier() const;
	const TypeSpecifier & get_type_specifier() const;
	const FunctionDefinitionArgList & get_arguments() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<UnsafeModifier> unsafe_modifier;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<FunctionDefinitionArgList> function_definition_arg_list;
	Gyoji::owned<Terminal> paren_r_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    class ClassMemberDeclarationDestructor : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassMemberDeclarationDestructor(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
	    Gyoji::owned<Terminal> _tilde_token,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<FunctionDefinitionArgList> _function_definition_arg_list,
	    Gyoji::owned<Terminal> _paren_r_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ClassMemberDeclarationDestructor();
	const AccessModifier & get_access_modifier() const;
	const UnsafeModifier & get_unsafe_modifier() const;
	const TypeSpecifier & get_type_specifier() const;
	const FunctionDefinitionArgList & get_arguments() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<UnsafeModifier> unsafe_modifier;
	Gyoji::owned<Terminal> tilde_token;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<FunctionDefinitionArgList> function_definition_arg_list;
	Gyoji::owned<Terminal> paren_r_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    class ClassMemberDeclaration : public Gyoji::frontend::ast::SyntaxNode {
    public:
	typedef std::variant<
	    Gyoji::owned<ClassMemberDeclarationVariable>,
	    Gyoji::owned<ClassMemberDeclarationMethod>,
	    Gyoji::owned<ClassMemberDeclarationConstructor>,
	    Gyoji::owned<ClassMemberDeclarationDestructor>,
	    Gyoji::owned<ClassDeclaration>,
	    Gyoji::owned<ClassDefinition>,
	    Gyoji::owned<EnumDefinition>,
	    Gyoji::owned<TypeDefinition>
	> MemberType;
	ClassMemberDeclaration(
	    MemberType _member,
	    const Gyoji::frontend::ast::SyntaxNode & _sn
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ClassMemberDeclaration();
	const ClassMemberDeclaration::MemberType & get_member();
    private:
	MemberType member;
    };
    
    class ClassMemberDeclarationList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassMemberDeclarationList(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~ClassMemberDeclarationList();
	const std::vector<Gyoji::owned<ClassMemberDeclaration>> & get_members() const;
	void add_member(Gyoji::owned<ClassMemberDeclaration> _member);
    private:
	std::vector<Gyoji::owned<ClassMemberDeclaration>> members;
    };
    
    /**
     * This is a forward declaration of a class which declares the
     * existence of a class, but does not specify the details of it.
     */
    class ClassDeclaration : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassDeclaration(
	    Gyoji::owned<ClassDeclStart> _class_decl_start,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ClassDeclaration();
	const AccessModifier & get_access_modifier() const;
	
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	
	const ClassArgumentList & get_argument_list() const;
    private:
	Gyoji::owned<ClassDeclStart> class_decl_start;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    class ClassDefinition : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ClassDefinition(
	    Gyoji::owned<ClassDeclStart> _class_decl_start,
	    Gyoji::owned<Terminal> _brace_l_token,
	    Gyoji::owned<ClassMemberDeclarationList> _class_member_declaration_list,
	    Gyoji::owned<Terminal> _brace_r_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ClassDefinition();
	const AccessModifier & get_access_modifier() const;
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	const ClassArgumentList & get_argument_list() const;
	const std::vector<Gyoji::owned<ClassMemberDeclaration>> & get_members() const;
    private:
	Gyoji::owned<ClassDeclStart> class_decl_start;
	Gyoji::owned<Terminal> brace_l_token;
	Gyoji::owned<ClassMemberDeclarationList> class_member_declaration_list;
	Gyoji::owned<Terminal> brace_r_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    class TypeDefinition : public Gyoji::frontend::ast::SyntaxNode {
    public:
	TypeDefinition(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<Terminal> _typedef_token,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~TypeDefinition();
	const AccessModifier & get_access_modifier() const;
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	const TypeSpecifier & get_type_specifier() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<Terminal> typedef_token;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    class EnumDefinitionValue : public Gyoji::frontend::ast::SyntaxNode {
    public:
	EnumDefinitionValue(
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _equals_token,
	    Gyoji::owned<Expression> _expression_primary,
	    Gyoji::owned<Terminal> _semicolon_token
	    );                          
	/**
	 * Destructor, nothing special.
	 */
	~EnumDefinitionValue();
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	const Expression & get_expression() const;
    private:
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> equals_token;
	Gyoji::owned<Expression> expression_primary;
	Gyoji::owned<Terminal> semicolon_token;      
    };
    
    class EnumDefinitionValueList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	EnumDefinitionValueList(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~EnumDefinitionValueList();
	void add_value(Gyoji::owned<EnumDefinitionValue>);
	const std::vector<Gyoji::owned<EnumDefinitionValue>> &get_values() const;
    private:
	std::vector<Gyoji::owned<EnumDefinitionValue>> values;
    };
    
    class EnumDefinition : public Gyoji::frontend::ast::SyntaxNode {
    public:
	EnumDefinition(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<Terminal> _enum_token,
	    Gyoji::owned<Terminal> _type_name_token,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<Terminal> _brace_l_token,
	    Gyoji::owned<EnumDefinitionValueList> _enum_value_list,
	    Gyoji::owned<Terminal> _brace_r_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~EnumDefinition();
	const AccessModifier & get_access_modifier() const;
	const std::string & get_type_name() const;
	const Gyoji::context::SourceReference & get_type_name_source_ref() const;
	
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	
	const EnumDefinitionValueList & get_value_list() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<Terminal> enum_token;
	Gyoji::owned<Terminal> type_name_token;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<Terminal> brace_l_token;
	Gyoji::owned<EnumDefinitionValueList> enum_value_list;
	Gyoji::owned<Terminal> brace_r_token;
	Gyoji::owned<Terminal> semicolon_token;
	
    };
    
    class ExpressionPrimaryIdentifier : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPrimaryIdentifier(Gyoji::owned<Terminal> _identifier_token);
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPrimaryIdentifier();
	const Terminal & get_identifier() const;
    private:
	Gyoji::owned<Terminal> identifier_token;
    };
    class ExpressionPrimaryNested : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPrimaryNested(
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _paren_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPrimaryNested();
	const Expression & get_expression() const;
    private:
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> paren_r_token;
    };
    
    class ExpressionPrimaryLiteralInt : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPrimaryLiteralInt(
	    Gyoji::owned<Terminal> literal_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPrimaryLiteralInt();
        /**
	 * Returns the string literal exactly as it appeared in the source-file.
	 */
	const std::string & get_value() const;
	const Terminal & get_literal_int_token() const;
	const Gyoji::context::SourceReference & get_value_source_ref() const;
    private:
	Gyoji::owned<Terminal> literal_token;
    };
    class ExpressionPrimaryLiteralChar : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPrimaryLiteralChar(
	    Gyoji::owned<Terminal> _literal_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPrimaryLiteralChar();
	std::string get_value() const;
	const Gyoji::context::SourceReference & get_value_source_ref() const;
    private:
	Gyoji::owned<Terminal> literal_token;
    };
    class ExpressionPrimaryLiteralString : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPrimaryLiteralString(
	    Gyoji::owned<Terminal> _literal_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPrimaryLiteralString();
	std::string get_value() const;
	const Gyoji::context::SourceReference & get_value_source_ref() const;
	void add_string(Gyoji::owned<Terminal> _added);
    private:
	Gyoji::owned<Terminal> literal_token;
	std::vector<Gyoji::owned<Terminal>> additional_strings;
    };
    class ExpressionPrimaryLiteralFloat : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPrimaryLiteralFloat(
	    Gyoji::owned<Terminal> _literal_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPrimaryLiteralFloat();
	
	/**
	 * For float literals, this returns only
	 * the part of the string that includes
	 * the number, not the part that encodes
	 * the type.  For example, in 178f32, this
	 * returns only the '178' part of it.
	 */
	const std::string & get_value() const;
	/**
	 * For float literals, this returns
	 * only the part of the string that
	 * is the type.  If no type is given,
	 * the default is 'f64'.  For example,
	 * in the literal '178f32', this returns
	 * 'f64' and in '178.0' this returns
	 * the default of 'f64'.
	 */
	const std::string & get_type() const;
	const Gyoji::context::SourceReference & get_value_source_ref() const;
    private:
	Gyoji::owned<Terminal> literal_token;
	std::string float_part;
	std::string type_part;
    };

    class ExpressionPrimaryLiteralBool : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPrimaryLiteralBool(
	    Gyoji::owned<Terminal> _literal_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPrimaryLiteralBool();
	
	/**
	 * For bool literals, this is just
	 * the truth value of the literal.
	 */
	bool get_value() const;
    private:
	Gyoji::owned<Terminal> literal_token;
	bool value;
    };
    class ExpressionPrimaryLiteralNull : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPrimaryLiteralNull(
	    Gyoji::owned<Terminal> _literal_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPrimaryLiteralNull();
    private:
	Gyoji::owned<Terminal> literal_token;
    };

    class ExpressionPostfixArrayIndex : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPostfixArrayIndex(
	    Gyoji::owned<Expression> _array_expression,
	    Gyoji::owned<Terminal> _bracket_l_token,
	    Gyoji::owned<Expression> _index_expression,
	    Gyoji::owned<Terminal> _bracket_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPostfixArrayIndex();
	const Expression & get_array() const;
	const Expression & get_index() const;
    private:
	Gyoji::owned<Expression> array_expression;
	Gyoji::owned<Terminal> bracket_l_token;
	Gyoji::owned<Expression> index_expression;
	Gyoji::owned<Terminal> bracket_r_token;
    };
    
    class ArgumentExpressionList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ArgumentExpressionList(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~ArgumentExpressionList();
	const std::vector<Gyoji::owned<Expression>> & get_arguments() const;
	void add_argument(Gyoji::owned<Expression> _argument);
	void add_argument(Gyoji::owned<Terminal> _comma_token, Gyoji::owned<Expression> _argument);
    private:
	std::vector<Gyoji::owned<Terminal>> comma_list;
	std::vector<Gyoji::owned<Expression>> arguments;
    };
    
    class ExpressionPostfixFunctionCall : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPostfixFunctionCall(
	    Gyoji::owned<Expression> _function_expression,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<ArgumentExpressionList> _arguments,
	    Gyoji::owned<Terminal> _paren_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPostfixFunctionCall();
	const Expression & get_function() const;
	const ArgumentExpressionList & get_arguments() const;
    private:
	Gyoji::owned<Expression> function_expression;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<ArgumentExpressionList> arguments;
	Gyoji::owned<Terminal> paren_r_token;
    };
    
    class ExpressionPostfixDot : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPostfixDot(
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _dot_token,
	    Gyoji::owned<Terminal> _identifier_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPostfixDot();
	const Expression & get_expression() const;
	const Terminal & get_identifier() const;
    private:
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> dot_token;
	Gyoji::owned<Terminal> identifier_token;
    };
    
    class ExpressionPostfixArrow : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionPostfixArrow(
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _arrow_token,
	    Gyoji::owned<Terminal> _identifier_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPostfixArrow();
	const Expression & get_expression() const;
	const Terminal & get_identifier() const;
    private:
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> arrow_token;
	Gyoji::owned<Terminal> identifier_token;
    };
    
    class ExpressionPostfixIncDec : public Gyoji::frontend::ast::SyntaxNode {
    public:
	typedef enum {
	    INCREMENT,
	    DECREMENT
	} OperationType;
	ExpressionPostfixIncDec(
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _operator_token,
	    ExpressionPostfixIncDec::OperationType _type                              
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionPostfixIncDec();
	const ExpressionPostfixIncDec::OperationType & get_type() const;
	const Expression & get_expression() const;
    private:
	ExpressionPostfixIncDec::OperationType type;
	Gyoji::owned<Terminal> operator_token;
	Gyoji::owned<Expression> expression;
    };
    
    class ExpressionUnaryPrefix : public Gyoji::frontend::ast::SyntaxNode {
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
	    Gyoji::owned<Terminal> _operator_token,
	    Gyoji::owned<Expression> _expression,
	    ExpressionUnaryPrefix::OperationType _type
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionUnaryPrefix();
	const ExpressionUnaryPrefix::OperationType & get_type() const;
	const Gyoji::context::SourceReference & get_operator_source_ref() const;
	const Expression & get_expression() const;
    private:
	ExpressionUnaryPrefix::OperationType type;
	Gyoji::owned<Terminal> operator_token;
	Gyoji::owned<Expression> expression;
    };
    
    class ExpressionUnarySizeofType : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionUnarySizeofType(
	    Gyoji::owned<Terminal> _sizeof_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _paren_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionUnarySizeofType();
	const TypeSpecifier & get_type_specifier() const;
    private:
	Gyoji::owned<Terminal> sizeof_token;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> paren_r_token;
    };
    
    class ExpressionCast : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionCast(
	    Gyoji::owned<Terminal> _cast_token,
	    Gyoji::owned<Terminal> _paren_l_token,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _comma_token,
	    Gyoji::owned<Expression> _expression,
	    Gyoji::owned<Terminal> _paren_r_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionCast();
	const TypeSpecifier & get_type() const;
	const Expression & get_expression() const;
    private:
	Gyoji::owned<Terminal> cast_token;
	Gyoji::owned<Terminal> paren_l_token;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> comma_token;
	Gyoji::owned<Expression> expression;
	Gyoji::owned<Terminal> paren_r_token;
    };
    
    class ExpressionBinary : public Gyoji::frontend::ast::SyntaxNode {
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
	    
	    // Bitwise
	    BITWISE_AND,
	    BITWISE_OR,
	    BITWISE_XOR,
	    SHIFT_LEFT,
	    SHIFT_RIGHT,
	    
	    // Relational
	    COMPARE_LESS,
	    COMPARE_GREATER,
	    COMPARE_LESS_EQUAL,
	    COMPARE_GREATER_EQUAL,
	    COMPARE_EQUAL,
	    COMPARE_NOT_EQUAL,
	    
	    // Assignment
	    ASSIGNMENT,
	    MUL_ASSIGNMENT,
	    DIV_ASSIGNMENT,
	    MOD_ASSIGNMENT,
	    ADD_ASSIGNMENT,
	    SUB_ASSIGNMENT,
	    LEFT_ASSIGNMENT,
	    RIGHT_ASSIGNMENT,
	    AND_ASSIGNMENT,
	    XOR_ASSIGNMENT,
	    OR_ASSIGNMENT
	} OperationType;
	ExpressionBinary(
	    Gyoji::owned<Expression> _expression_a,
	    Gyoji::owned<Terminal> _operator_token,
	    Gyoji::owned<Expression> _expression_b,
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
	    Gyoji::owned<Expression> _expression_a,
	    Gyoji::owned<Terminal> _operator_token,
	    Gyoji::owned<Terminal> _operator_token2,
	    Gyoji::owned<Expression> _expression_b,
	    ExpressionBinary::OperationType _type
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionBinary();
	const Expression & get_a() const;
	const ExpressionBinary::OperationType & get_operator() const;
	const Gyoji::context::SourceReference & get_operator_source_ref() const;
	const Expression & get_b() const;
	
    private:
	OperationType type;
	Gyoji::owned<Expression> expression_a;
	Gyoji::owned<Terminal> operator_token;
	Gyoji::owned<Terminal> operator_token2;
	Gyoji::owned<Expression> expression_b;
    };
    
    class ExpressionTrinary : public Gyoji::frontend::ast::SyntaxNode {
    public:
	ExpressionTrinary(
	    Gyoji::owned<Expression> _condition,
	    Gyoji::owned<Terminal> _questionmark_token,
	    Gyoji::owned<Expression> _if_expression,
	    Gyoji::owned<Terminal> _colon_token,
	    Gyoji::owned<Expression> _else_expression
	    );
	/**
	 * Destructor, nothing special.
	 */
	~ExpressionTrinary();
	const Expression & get_condition() const;
	const Expression & get_if() const;
	const Expression & get_else() const;
    private:
	Gyoji::owned<Expression> condition;
	Gyoji::owned<Terminal> questionmark_token;
	Gyoji::owned<Expression> if_expression;
	Gyoji::owned<Terminal> colon_token;
	Gyoji::owned<Expression> else_expression;      
    };
    
    class Expression : public Gyoji::frontend::ast::SyntaxNode {
    public:
	typedef std::variant<
			     Gyoji::owned<ExpressionPrimaryIdentifier>,
			     Gyoji::owned<ExpressionPrimaryNested>,
			     Gyoji::owned<ExpressionPrimaryLiteralChar>,
			     Gyoji::owned<ExpressionPrimaryLiteralString>,
			     Gyoji::owned<ExpressionPrimaryLiteralInt>,
			     Gyoji::owned<ExpressionPrimaryLiteralFloat>,
			     Gyoji::owned<ExpressionPrimaryLiteralBool>,
			     Gyoji::owned<ExpressionPrimaryLiteralNull>,
			     Gyoji::owned<ExpressionPostfixArrayIndex>,
			     Gyoji::owned<ExpressionPostfixFunctionCall>,
			     Gyoji::owned<ExpressionPostfixDot>,
			     Gyoji::owned<ExpressionPostfixArrow>,
			     Gyoji::owned<ExpressionPostfixIncDec>,
			     Gyoji::owned<ExpressionUnaryPrefix>,
			     Gyoji::owned<ExpressionUnarySizeofType>,
			     Gyoji::owned<ExpressionBinary>,
			     Gyoji::owned<ExpressionTrinary>,
			     Gyoji::owned<ExpressionCast>
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
    
    class GlobalInitializerExpressionPrimary : public Gyoji::frontend::ast::SyntaxNode {
    public:
	GlobalInitializerExpressionPrimary(
	    Gyoji::owned<Terminal> _equals_token,
	    Gyoji::owned<Expression> _expression
	    );
	/**
	 * Destructor, nothing special.
	 */
	~GlobalInitializerExpressionPrimary();
	const Expression & get_expression() const;
    private:
	Gyoji::owned<Terminal> equals_token;
	Gyoji::owned<Expression> expression;
	
    };
    class GlobalInitializerAddressofExpressionPrimary : public Gyoji::frontend::ast::SyntaxNode {
    public:
	GlobalInitializerAddressofExpressionPrimary(
	    Gyoji::owned<Terminal> _equals_token,
	    Gyoji::owned<Terminal> _addressof_token,
	    Gyoji::owned<Expression> _expression
	    );
	/**
	 * Destructor, nothing special.
	 */
	~GlobalInitializerAddressofExpressionPrimary();
	const Expression & get_expression() const;
    private:
	Gyoji::owned<Terminal> equals_token;
	Gyoji::owned<Terminal> addressof_token;
	Gyoji::owned<Expression> expression;
    };
    
    class StructInitializer : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StructInitializer(
	    Gyoji::owned<Terminal> _dot_token,
	    Gyoji::owned<Terminal> _identifier_token,
	    Gyoji::owned<GlobalInitializer> _global_initializer,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~StructInitializer();
	const GlobalInitializer & get_initializer() const;
    private:
	Gyoji::owned<Terminal> dot_token;
	Gyoji::owned<Terminal> identifier_token;
	Gyoji::owned<GlobalInitializer> global_initializer;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    class StructInitializerList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	StructInitializerList(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~StructInitializerList();
	void add_initializer(Gyoji::owned<StructInitializer> initializer);
	const std::vector<Gyoji::owned<StructInitializer>> & get_initializers() const;
    private:
	std::vector<Gyoji::owned<StructInitializer>> initializers;
    };
    
    class GlobalInitializerStructInitializerList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	GlobalInitializerStructInitializerList(
	    Gyoji::owned<Terminal> _equals_token,
	    Gyoji::owned<Terminal> _brace_l_token,
	    Gyoji::owned<StructInitializerList> _struct_initializer,
	    Gyoji::owned<Terminal> _brace_r_token
	    );
	
	/**
	 * Destructor, nothing special.
	 */
	~GlobalInitializerStructInitializerList();
	const StructInitializerList & get_struct_initializer() const;
    private:
	Gyoji::owned<Terminal> equals_token;
	Gyoji::owned<Terminal> brace_l_token;
	Gyoji::owned<StructInitializerList> struct_initializer;
	Gyoji::owned<Terminal> brace_r_token;
    };
    
    class GlobalInitializer : public Gyoji::frontend::ast::SyntaxNode {
    public:
	typedef std::variant<
        Gyoji::owned<GlobalInitializerExpressionPrimary>,
        Gyoji::owned<GlobalInitializerAddressofExpressionPrimary>,
        Gyoji::owned<GlobalInitializerStructInitializerList>,
        nullptr_t> GlobalInitializerType;
	GlobalInitializer(const Gyoji::context::SourceReference & _source_ref);
	GlobalInitializer(GlobalInitializerType initializer, const Gyoji::frontend::ast::SyntaxNode & _sn);
	/**
	 * Destructor, nothing special.
	 */
	~GlobalInitializer();
	const GlobalInitializerType & get_initializer() const;
    private:
	GlobalInitializerType initializer;
    };
    
    class FileStatementGlobalDefinition : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FileStatementGlobalDefinition(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
	    Gyoji::owned<TypeSpecifier> _type_specifier,
	    Gyoji::owned<Terminal> _name,
	    Gyoji::owned<GlobalInitializer> _global_initializer,
	    Gyoji::owned<Terminal> _semicolon
	    );
	/**
	 * Destructor, nothing special.
	 */
	~FileStatementGlobalDefinition();
	const AccessModifier & get_access_modifier() const;
	const UnsafeModifier & get_unsafe_modifier() const;
	const TypeSpecifier & get_type_specifier() const;
	const std::string & get_name() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	const GlobalInitializer & get_global_initializer() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<UnsafeModifier> unsafe_modifier;
	Gyoji::owned<TypeSpecifier> type_specifier;
	Gyoji::owned<Terminal> name; // function name (IDENTIFIER)
	Gyoji::owned<GlobalInitializer> global_initializer;
	Gyoji::owned<Terminal> semicolon;
    };
    
    class NamespaceDeclaration : public Gyoji::frontend::ast::SyntaxNode {
    public:
	NamespaceDeclaration(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<Terminal> _namespace_token,
	    Gyoji::owned<Terminal> _identifier_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~NamespaceDeclaration();
	const AccessModifier & get_access_modifier() const;
	const Terminal & get_name() const;
    private:
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<Terminal> namespace_token;
	Gyoji::owned<Terminal> identifier_token;
    };
    
    class FileStatementNamespace : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FileStatementNamespace(
	    Gyoji::owned<NamespaceDeclaration> _namespace_declaration,
	    Gyoji::owned<Terminal> _brace_l_token,
	    Gyoji::owned<FileStatementList> _file_statement_list,
	    Gyoji::owned<Terminal> _brace_r_token,
	    Gyoji::owned<Terminal> _semicolon_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~FileStatementNamespace();
	const NamespaceDeclaration & get_declaration() const;
	const FileStatementList & get_statement_list() const;
    private:
	Gyoji::owned<NamespaceDeclaration> namespace_declaration;
	Gyoji::owned<Terminal> brace_l_token;
	Gyoji::owned<FileStatementList> file_statement_list;
	Gyoji::owned<Terminal> brace_r_token;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    class UsingAs : public Gyoji::frontend::ast::SyntaxNode {
    public:
	UsingAs(
	    Gyoji::owned<Terminal> _as_token,
	    Gyoji::owned<Terminal> _identifier_token
	    );
	UsingAs(const Gyoji::context::SourceReference & _source_ref);
	/**
	 * Destructor, nothing special.
	 */
	~UsingAs();
	bool is_as() const;
	const std::string & get_using_name() const;
	const Gyoji::context::SourceReference & get_using_name_source_ref() const;
    private:
	bool aas;
	std::string using_name;
	Gyoji::owned<Terminal> as_token;
	Gyoji::owned<Terminal> identifier_token;
    };
    
    class FileStatementUsing : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FileStatementUsing(
	    Gyoji::owned<AccessModifier> _access_modifier,
	    Gyoji::owned<Terminal> _using,
	    Gyoji::owned<Terminal> _namespace,
	    Gyoji::owned<Terminal> _namespace_name,
	    Gyoji::owned<UsingAs> _using_as,
	    Gyoji::owned<Terminal> _semicolon);
	/**
	 * Destructor, nothing special.
	 */
	~FileStatementUsing();
    private:
	const AccessModifier & get_access_modifier() const;
	std::string get_namespace() const;
	const Gyoji::context::SourceReference & get_namespace_source_ref() const;
	const UsingAs &get_using_as() const;
	
	Gyoji::owned<AccessModifier> access_modifier;
	Gyoji::owned<Terminal> using_token;
	Gyoji::owned<Terminal> namespace_token;
	Gyoji::owned<Terminal> namespace_name_token;
	Gyoji::owned<UsingAs> using_as;
	Gyoji::owned<Terminal> semicolon_token;
    };
    
    /**
     * This class represents a list of statements at the file-level.
     * These statements are things like global variable declarations,
     * function declarations, function definitions, and type definitions
     * that appear at the top-level of the translation unit or
     * possibly nested inside namespaces at the translation unit level.
     */
    class FileStatementList : public Gyoji::frontend::ast::SyntaxNode {
    public:
	FileStatementList(const Gyoji::context::SourceReference & _source_ref);
	FileStatementList(Gyoji::owned<Terminal> _yyeof);
	/**
	 * Destructor, nothing special.
	 */
	~FileStatementList();
	const std::vector<Gyoji::owned<FileStatement>> & get_statements() const;
	void add_statement(Gyoji::owned<FileStatement> statement);
    private:
	Gyoji::owned<Terminal> yyeof;
	std::vector<Gyoji::owned<FileStatement>> statements;
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
    class TranslationUnit : public Gyoji::frontend::ast::SyntaxNode {
    public:
	TranslationUnit(
	    Gyoji::owned<FileStatementList> file_statement_list,
	    Gyoji::owned<Terminal> yyeof_token
	    );
	/**
	 * Destructor, nothing special.
	 */
	~TranslationUnit();
	/**
	 * This method returns a list of 'const' owned pointers to the
	 * statements defined in this translation unit.
	 */
	const std::vector<Gyoji::owned<FileStatement>> & get_statements() const;
    private:
	Gyoji::owned<FileStatementList> file_statement_list;
	Gyoji::owned<Terminal> yyeof_token;
	
    };
    
};

/*! @} End of Doxygen Groups*/
