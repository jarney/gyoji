#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir/types.hpp>
#include <jlang-context.hpp>

#include <string>
#include <map>
#include <vector>

namespace JLang::mir {
    /**
     * @brief Operations inside basic blocks, the virtual instruction-set of the MIR.
     *
     * @details
     * This represents a variation on the
     * "Three Address Code" form.  These
     * operations make up the virtual instruction-set
     * of the MIR virtual machine.
     *
     * Each operation such as 'add' c =  a + b is represented as
     * @code{.unparsed}
     *     _c = add _a, _b
     * @endcode
     *
     * where '_c' is the returned value and
     * '_a' and '_b' are the operands.
     *
     * There are some
     * operations (like function calls) that take
     * more than three addresses, but for most,
     * the operation performs an operation on
     * one or two operands and returns a result.
     *
     * Some operations carry additional information
     * such as branch locations (basic-block IDs)
     * or string literals (in the case of the literal-string opcode).
     *
     * Each operation has a "Type" field representing the
     * concrete operation being performed.
     * The operands and result are passed in
     * and manipulated.
     *
     * The operands are identified in terms of
     * the ID of the temporary value they operate on.
     * Type information is carried there, not inside
     * the operation although operations do have
     * strict conditions on the types of the operands
     * and the types of the returned values.  Those
     * conditions are enforced outside of this
     * structure.
     */
    class Operation {
    public:
	typedef enum {
	    // Global symbols
	    /**
	     * @brief Function call
	     * @details
	     * Directs the MIR to call a function.  The first
	     * operand is the function to call and the remaining
	     * operands are the arguments to the function.
	     *
	     * The opcode will return the same type that the
	     * function returns and the operand types will match
	     * the arguments of the function.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * Symbols:
	     *    main : u8(*)(u32, u8**)
	     * Operations:
	     * _0: (u8)(*)(u32, u8**) = symbol("main")
	     * _1: u32 = literal-int(0)
	     * _2: u8** = nullptr
	     * _3: u8 = function-call (_0, _1, _2)
	     * @endcode
	     */
	    OP_FUNCTION_CALL,
	    /**
	     * @brief Load a symbol from the symbol-table.
	     * @details
	     * This opcode directs the MIR to load a symbol
	     * from the symbol table.  This will be either
	     * a function pointer (to be used by the OP_FUNCTION_CALL opcode)
	     * or by another opcode in evaluating an expression like
	     * a global variable.
	     *
	     * This opcode will return the type of the symbol found.
	     * In the case of a function, this is the function-pointer
	     * type signature and in the case of a global variable, it will
	     * be whatever type that variable is declared as.
	     *
	     * @code{.unparsed}
	     * Symbols:
	     *    main : u8(*)(u32, u8**)
	     * Operations:
	     * _0: (u8)(*)(u32, u8**) = symbol("main")
	     * _1: u32 = literal-int(0)
	     * _2: u8** = nullptr
	     * _3: u8 = function-call (_0, _1, _2)
	     * @endcode
	     */
	    OP_SYMBOL,

	    // Cast operations
	    /**
	     * @brief Widen a signed integer to a larger type.
	     * @details
	     * This opcode widens a value from a signed integer
	     * to another signed integer type by "sign-extending" the
	     * value.  For example, it can convert a signed i8 to a
	     * signed i32.  This operation takes a single
	     * operand, the variable to be extended, and a type
	     * to which it should be extended.  The type should also
	     * be a signed integer.
	     *
	     * The result will be the value of the integer, sign-extended
	     * to the type given.
	     *
	     * It is an error to pass any operand which is not a signed
	     * integer or a Type which is not a signed integer type.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * _0 : i8
	     * _1: i16 = widen-signed ( _0, i16 )
	     * _2: i32 = widen-signed ( _1, i32 )
	     * _3: i64 = widen-signed ( _2, i64 )
	     * @endcode
	     */
	    OP_WIDEN_SIGNED,    // Widen a signed integer to a larger type.

	    /**
	     * @brief Widen an unsigned integer to a larger type.
	     * @details
	     * This opcode widens a value from an unsigned integer
	     * to another unsigned integer type by "zero-extending" the
	     * value.  For example, it can convert an unsigned u8
	     * to a u64 by zero-extending it.
	     *
	     * The result will be the value of the unsigned integer,
	     * zero-extended to the type given.
	     *
	     * It is an error to pass any operand which is not
	     * an unsigned integer or a Type which is not an unsigned
	     * integer type.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * _0 : u8
	     * _1: u16 = widen-unsigned ( _0, u16 )
	     * _2: u32 = widen-unsigned ( _1, u32 )
	     * _3: u64 = widen-unsigned ( _2, u64 )
	     * @endcode
	     */
	    OP_WIDEN_UNSIGNED,  // Widen an unsigned integer to a larger type.

	    /**
	     * @brief Widen a floating-point integer to wider type.
	     *
	     * @details
	     * This opcode widens a value from a floating-point (f32)
	     * to a double-precision floating point (f64).
	     *
	     * The result will be the value of the floating-point,
	     * converted and widened to larger type (f64).
	     *
	     * It is an error to pass any operand which is not
	     * a floating-point number of a Type which is not a floating-point
	     * number.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * _0 : f32
	     * _1: f64 = widen-float ( _0, f64 )
	     * @endcode
	     */
	    OP_WIDEN_FLOAT,     // Widen a floating-point to a larger type.

	    // Indirect access
	    OP_ARRAY_INDEX,     // Pointer types -> Pointer Target type
	    OP_DOT,             // class types -> Found member type
	    OP_ARROW,           // Pointer to class types -> Found member type

	    // Variable access
	    /**
	     * @brief Declare local variable
	     *
	     * @details
	     * This declares a local variable on the stack of a given type.  This
	     * takes no opcodes and returns no value.  Instead, it operates on the
	     * virtual machine by bringing a variable into scope so that it can
	     * be loaded and stored by other opcodes.
	     *
	     * This opcode has the name of the variable to declare and the
	     * type of variable it is so that it can be appropriately allocated.
	     */
	    OP_LOCAL_DECLARE,   // N/A

	    /**
	     * @breif Undeclare local variable
	     * @details
	     * This de-allocates a local variable on the stack of a given type.  This
	     * takes no opcodes and returns no value.  Instead, it operates
	     * on the virtual machine by removing that variable from scope.  Access
	     * to this variable after it has gone out of scope is an error.
	     *
	     * This opcode carries the name of the variable to de-allocate.
	     */
	    OP_LOCAL_UNDECLARE, // N/A
	    /**
	     * @brief Load value from variable.
	     * @details
	     * This loads the value of a variable from its storage and places
	     * the result in the return-value.
	     *
	     * @code{.unparsed}
	     * RET: variable-type = load (variable)
	     * @endcode
	     *
	     * This opcode takes no operands.
	     */
	    OP_LOCAL_VARIABLE,
	    
            // Literals
	    /**
	     * @brief Loads a literal character constant.
	     *
	     * @details
	     * This loads a literal character constant into
	     * the return-value of the operation.
	     *
	     * @code{.unparsed}
	     * RET: u8 = literal-char( 'x' )
	     * @endcode
	     */
	    OP_LITERAL_CHAR,
	    /**
	     * @brief Loads a string constant.
	     *
	     * @details
	     * This loads a pointer to a u8 that points to a
	     * string literal defined in the static constant
	     * data area.
	     *
	     * @code{.unparsed}
	     * RET: u8* = literal-string( "Hello World" )
	     * @endcode
	     *
	     */
	    OP_LITERAL_STRING,
	    OP_LITERAL_INT,
	    OP_LITERAL_FLOAT,
	    
            // Unary operations
	    OP_POST_INCREMENT,  // Pointer types, integer types -> Operand type
	    OP_POST_DECREMENT,  // Pointer types, integer types -> Operand type
	    OP_PRE_INCREMENT,   // Pointer types, integer types -> Operand type
	    OP_PRE_DECREMENT,   // Pointer types, integer types -> Operand type
	    OP_ADDRESSOF,   // Any type -> Pointer type
	    OP_DEREFERENCE, // Pointer types -> Pointer Target type
	    OP_NEGATE,      // Numeric types -> Numeric type
	    OP_BITWISE_NOT, // Integer types -> Integer Type
	    OP_LOGICAL_NOT, // Boolean types -> Boolean
	    OP_SIZEOF_TYPE, // Any type -> u64

	    // Binary operations: arithmetic
	    OP_ADD,         // Matching Numeric types -> Operand type
	    OP_SUBTRACT,    // Matching Numeric types -> Operand type
	    OP_MULTIPLY,    // Matching Numeric types -> Operand type
	    OP_DIVIDE,      // Matching Integer types -> Operand type
	    OP_MODULO,      // Matching Integer types -> Operand type

	    // Binary operations: logical
	    OP_LOGICAL_AND, // Boolean types -> Boolean
	    OP_LOGICAL_OR,  // Boolean types -> Boolean

	    // Binary operations: bitwise
	    OP_BITWISE_AND, // Matching integer types -> Integer type
	    OP_BITWISE_OR,  // Matching integer types -> Integer type
	    OP_BITWISE_XOR, // Matching integer types -> Integer type
	    OP_SHIFT_LEFT,  // Matching integer types -> Integer type
	    OP_SHIFT_RIGHT, // Matching integer types -> Integer type

	    // Binary operations: comparisons
	    OP_COMPARE_LT,  // Primitive types -> Boolean
	    OP_COMPARE_GT,  // Primitive types -> Boolean
	    OP_COMPARE_LE,  // Primitive types -> Boolean
	    OP_COMPARE_GE,  // Primitive types -> Boolean
	    OP_COMPARE_NE,  // Pointer types, Primitive types, recursive for class types.
	    OP_COMPARE_EQ,  // Pointer types, Primitive types, recursive for class types.
	    
	    // Binary operations: assignments
	    OP_ASSIGN,      // Primitive types, pointer types, recursive for class types.

	    // Branch and flow control
	    OP_JUMP_IF_EQUAL, // Boolean types
	    OP_JUMP,
	    OP_RETURN
	} OperationType;
	Operation(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result
	    );
	Operation(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	Operation(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand_a,
	    size_t _operand_b
	    );
	Operation(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand_a,
	    size_t _operand_b,
	    size_t _operand_c
	    );
	Operation(
	    const Operation & _other
	    );
	virtual ~Operation();

	virtual std::string get_description() const;
	void dump(FILE *out) const;
	void add_operand(size_t operand);
	OperationType get_type() const;
	const std::vector<size_t> & get_operands() const;
	size_t get_result() const;

	const JLang::context::SourceReference & get_source_ref() const;
	
    protected:
	OperationType type;
	const JLang::context::SourceReference & src_ref;
	std::vector<size_t> operands;
	size_t result;
    };

    class OperationUnary : public Operation {
    public:
	OperationUnary(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	virtual ~OperationUnary();
	size_t get_a() const;
    };
    
    class OperationCast : public OperationUnary {
    public:
	OperationCast(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand,
	    const Type *_cast_type
	    );
	virtual ~OperationCast();
	const Type* get_cast_type() const;
	virtual std::string get_description() const;
    private:
	const Type *cast_type;
    };

    class OperationBinary : public Operation {
    public:
	OperationBinary(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand_a,
	    size_t _operand_b
	    );
	virtual ~OperationBinary();
	size_t get_a() const;
	size_t get_b() const;
    };

    class OperationFunctionCall : public Operation {
    public:
	OperationFunctionCall(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _callee_tmpvar
	    );
	virtual ~OperationFunctionCall();
    };
    
    class OperationSymbol : public Operation {
    public:
	OperationSymbol(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _symbol_name
	    );
	virtual ~OperationSymbol();
	const std::string & get_symbol_name() const;
	
	virtual std::string get_description() const;
    private:
	const std::string symbol_name;
    };

    class OperationArrayIndex : public Operation {
    public:
	OperationArrayIndex(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _index_tmpvar,
	    const Type * _array_type
	    );
	virtual ~OperationArrayIndex();
	const Type *get_array_type() const;
	
	virtual std::string get_description() const;
    private:
	const Type *array_type;
    };

    class OperationDot : public Operation {
    public:
	OperationDot(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand,
	    std::string _member_name
	    );
	virtual ~OperationDot();
	const std::string & get_member_name() const;
	
	virtual std::string get_description() const;
    private:
	const std::string member_name;
    };

    class OperationArrow : public Operation {
    public:
	OperationArrow(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand,
	    std::string _member_name
	    );
	virtual ~OperationArrow();
	const std::string & get_member_name() const;
	
	virtual std::string get_description() const;
    private:
	const std::string member_name;
    };

    class OperationLocalVariable : public Operation {
    public:
	OperationLocalVariable(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _symbol_name,
	    const Type * _var_type);
	virtual ~OperationLocalVariable();
	const std::string & get_symbol_name() const;
	const Type * get_var_type() const;
	
	virtual std::string get_description() const;
    private:
	const std::string symbol_name;
	const Type * var_type;
    };

    class OperationLiteralChar : public Operation {
    public:
	OperationLiteralChar(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _literal_char
	    );
	virtual ~OperationLiteralChar();
	const std::string & get_literal_char() const;
	virtual std::string get_description() const;
    private:
	const std::string literal_char;
    };
    class OperationLiteralString : public Operation {
    public:
	OperationLiteralString(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _literal_string
	    );
	virtual ~OperationLiteralString();
	const std::string & get_literal_string() const;
	virtual std::string get_description() const;
    private:
	const std::string literal_string;
    };
    class OperationLiteralInt : public Operation {
    public:
	OperationLiteralInt(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _literal_int
	    );
	virtual ~OperationLiteralInt();
	const std::string & get_literal_int() const;
	virtual std::string get_description() const;
    private:
	const std::string literal_int;
    };
    class OperationLiteralFloat : public Operation {
    public:
	OperationLiteralFloat(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _literal_float
	    );
	virtual ~OperationLiteralFloat();
	const std::string & get_literal_float() const;
	virtual std::string get_description() const;
    private:
	const std::string literal_float;
    };
    
    class OperationBitwiseNot : public Operation {
    public:
	OperationBitwiseNot(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	virtual ~OperationBitwiseNot();
    private:
    };

    class OperationLogicalNot : public Operation {
    public:
	OperationLogicalNot(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	virtual ~OperationLogicalNot();
    private:
    };
    
    class OperationSizeofType : public Operation {
    public:
	OperationSizeofType(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	virtual ~OperationSizeofType();
    private:
    };

    class OperationJumpIfEqual : public Operation {
    public:
	OperationJumpIfEqual(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _operand,
	    size_t _if_block,
	    size_t _else_block
	    );
	virtual ~OperationJumpIfEqual();
	virtual std::string get_description() const;
    private:
    };
    
    class OperationJump : public Operation {
    public:
	OperationJump(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _block
	    );
	virtual ~OperationJump();
	virtual std::string get_description() const;
    private:
    };
    
    class OperationReturn : public Operation {
    public:
	OperationReturn(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _operand
	    );
	virtual ~OperationReturn();
	virtual std::string get_description() const;
    };

    class OperationLocalDeclare : public Operation {
    public:
	OperationLocalDeclare(
	    const JLang::context::SourceReference & _src_ref,
	    std::string _variable,
	    std::string _var_type
	    );
	virtual ~OperationLocalDeclare();
	const std::string & get_variable() const;
	const std::string & get_var_type() const;
	virtual std::string get_description() const;
    private:
	std::string variable;
	std::string var_type;
    };
    class OperationLocalUndeclare : public Operation {
    public:
	OperationLocalUndeclare(
	    const JLang::context::SourceReference & _src_ref,
	    std::string _variable
	    );
	virtual ~OperationLocalUndeclare();
	virtual std::string get_description() const;
    private:
	std::string variable;
    };

    
    void operation_static_init();
};
