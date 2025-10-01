#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir/types.hpp>
#include <jlang-context.hpp>

#include <string>
#include <map>
#include <vector>

namespace JLang::mir {
    // This is a basic operation in the MIR.
    // It can represent things like loading
    // variables, calling functions, incrementing
    // variables, etc.
    // a++ is represented as _a = INC _a
    // indicating that it increments 'a' and puts the
    // result back in 'a'.
    //
    // a + b is represented as
    // _c = ADD _a, _b
    //
    // We should look at the Rust MIR for inspiration
    // on the design of this representation.  Once we
    // have the basic operations, we can simply wire them up
    // to the MIR generation logic.
    //
    // Note that operations are "sometimes" type-aware
    // for example == operates on any type but always
    // returns a boolean, but the actual implementation/opcode of "=="
    // may depend on the types being compared.


    //! Operations inside basic blocks.
    /**
     * This represents a variation on the
     * "Three Address Code" form.  There are some
     * operations (like function calls) that take
     * more than three addresses, but for most,
     * the operation performs an operation on
     * one or two operands and returns a result.
     *
     * Each operation has a "Type" field representing the
     * concrete operation being performed.
     * The operands and result are passed in
     * and manipulated.
     *
     * The operands are identified in terms of
     * the ID of the temporary value they operate on.
     * Type information is carried there, not inside
     * the operation.
     */
    class Operation {
    public:
	typedef enum {
	    // Operation        // Validations -> Result
	    OP_FUNCTION_CALL,   // Any, but need to match function signatures -> Function return-value type
	    OP_SYMBOL,          // N/A

	    // Cast operations
	    OP_WIDEN_SIGNED,    // Widen a signed integer to a larger type.
	    OP_WIDEN_UNSIGNED,  // Widen an unsigned integer to a larger type.
	    OP_WIDEN_FLOAT,     // Widen a floating-point to a larger type.

	    // Indirect access
	    OP_ARRAY_INDEX,     // Pointer types -> Pointer Target type
	    OP_DOT,             // class types -> Found member type
	    OP_ARROW,           // Pointer to class types -> Found member type

	    // Variable access
	    OP_LOCAL_DECLARE,   // N/A
	    OP_LOCAL_UNDECLARE, // N/A
	    OP_LOCAL_VARIABLE,  // N/A
	    
            // Literals
	    OP_LITERAL_CHAR,    // N/A
	    OP_LITERAL_STRING,  // N/A
	    OP_LITERAL_INT,     // N/A
	    OP_LITERAL_FLOAT,   // N/A
	    
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
