#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir/types.hpp>

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
	    OP_IMMEDIATE,
	    OP_SYMBOL,
	    OP_ADD,
	    OP_MULTIPLY,
	    OP_DIVIDIE,
	    OP_SUBTRACT,
	    OP_ASSIGN
	} OperationType;
	Operation();
	~Operation();
	std::vector<size_t> operands;
	size_t result;
    private:
    };
};
