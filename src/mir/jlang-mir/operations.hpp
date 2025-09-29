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
	    OP_FUNCTION_CALL,
	    OP_SYMBOL,

	    OP_ARRAY_INDEX,
	    OP_DOT,
	    OP_ARROW,
	    
	    OP_LOCAL_DECLARE,
	    OP_LOCAL_UNDECLARE,
	    OP_LOCAL_VARIABLE,
	    OP_LITERAL_CHAR,
	    OP_LITERAL_STRING,
	    OP_LITERAL_INT,
	    OP_LITERAL_FLOAT,
	    OP_POST_INCREMENT,
	    OP_POST_DECREMENT,
	    OP_PRE_INCREMENT,
	    OP_PRE_DECREMENT,

	    OP_ADDRESSOF,
	    OP_DEREFERENCE,
	    OP_NEGATE,
	    OP_BITWISE_NOT,
	    OP_LOGICAL_NOT,

	    OP_SIZEOF_TYPE,
	    
	    OP_ADD,
	    OP_SUBTRACT,
	    OP_MULTIPLY,
	    OP_DIVIDE,
	    OP_MODULO,

	    OP_LOGICAL_AND,
	    OP_LOGICAL_OR,

	    OP_BITWISE_AND,
	    OP_BITWISE_OR,
	    OP_BITWISE_XOR,
	    OP_SHIFT_LEFT,
	    OP_SHIFT_RIGHT,

	    OP_COMPARE_LT,
	    OP_COMPARE_GT,
	    OP_COMPARE_LE,
	    OP_COMPARE_GE,
	    OP_COMPARE_NE,
	    OP_COMPARE_EQ,
	    
	    OP_ASSIGN,
	    
	    OP_JUMP_IF_EQUAL,
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
	    const Operation & _other
	    );
	~Operation();

	std::string get_name() const;
	void dump() const;
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

    class OperationFunctionCall : public Operation {
    public:
	OperationFunctionCall(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _callee_tmpvar
	    );
	~OperationFunctionCall();
    };
    
    class OperationSymbol : public Operation {
    public:
	OperationSymbol(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _symbol_name
	    );
	~OperationSymbol();
	const std::string & get_symbol_name() const;
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
	~OperationArrayIndex();
	const Type *get_array_type() const;
    private:
	const Type *array_type;
    };

    class OperationDot : public Operation {
    public:
	OperationDot(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _member_name
	    );
	~OperationDot();
	const std::string & get_member_name() const;
    private:
	const std::string member_name;
    };

    class OperationArrow : public Operation {
    public:
	OperationArrow(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _member_name
	    );
	~OperationArrow();
	const std::string & get_member_name() const;
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
	~OperationLocalVariable();
	const std::string & get_symbol_name() const;
	const Type * get_var_type() const;
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
	~OperationLiteralChar();
	const std::string & get_literal_char() const;
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
	~OperationLiteralString();
	const std::string & get_literal_string() const;
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
	~OperationLiteralInt();
	const std::string & get_literal_int() const;
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
	~OperationLiteralFloat();
	const std::string & get_literal_float() const;
    private:
	const std::string literal_float;
    };
    class OperationPreIncrement : public Operation {
    public:
	OperationPreIncrement(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationPreIncrement();
    private:
    };
    
    class OperationPostIncrement : public Operation {
    public:
	OperationPostIncrement(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationPostIncrement();
    private:
    };
    class OperationPreDecrement : public Operation {
    public:
	OperationPreDecrement(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationPreDecrement();
    private:
    };
    
    class OperationPostDecrement : public Operation {
    public:
	OperationPostDecrement(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationPostDecrement();
    private:
    };

    class OperationAddressOf : public Operation {
    public:
	OperationAddressOf(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationAddressOf();
    private:
    };
    
    class OperationDereference : public Operation {
    public:
	OperationDereference(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationDereference();
    private:
    };
    
    class OperationNegate : public Operation {
    public:
	OperationNegate(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationNegate();
    private:
    };

    class OperationBitwiseNot : public Operation {
    public:
	OperationBitwiseNot(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationBitwiseNot();
    private:
    };

    class OperationLogicalNot : public Operation {
    public:
	OperationLogicalNot(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationLogicalNot();
    private:
    };
    
    class OperationSizeofType : public Operation {
    public:
	OperationSizeofType(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	~OperationSizeofType();
    private:
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
	~OperationBinary();
	size_t get_a() const;
	size_t get_b() const;
    };

    class OperationJumpIfEqual : public Operation {
    public:
	OperationJumpIfEqual(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _operand,
	    std::string _label
	    );
	~OperationJumpIfEqual();
    private:
	std::string label;
    };
    
    class OperationJump : public Operation {
    public:
	OperationJump(
	    const JLang::context::SourceReference & _src_ref,
	    std::string _label
	    );
	~OperationJump();
    private:
	std::string label;
    };
    
    class OperationReturn : public Operation {
    public:
	OperationReturn(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _operand
	    );
	~OperationReturn();
    };

    class OperationLocalDeclare : public Operation {
    public:
	OperationLocalDeclare(
	    const JLang::context::SourceReference & _src_ref,
	    std::string _variable,
	    std::string _var_type
	    );
	~OperationLocalDeclare();
	const std::string & get_variable() const;
	const std::string & get_var_type() const;
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
	~OperationLocalUndeclare();
    private:
	std::string variable;
    };

    
    void operation_static_init();
};
