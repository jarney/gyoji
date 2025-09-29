#pragma once

#include <jlang-mir/types.hpp>
#include <jlang-mir/operations.hpp>
#include <jlang-mir/functions.hpp>
#include <jlang-mir/symbols.hpp>

namespace JLang::mir {
    /**
     * The MIR is an intermediate representation
     * of a source program.  It contains no syntax
     * information and just enough context from the
     * source-code that any errors can be traced back
     * to the source-code for quality error reporting.
     *
     * The purpose of the MIR is to provide a representation
     * that is at a low enough level that a code-generator
     * such as LLVM can be used to emit assembly,
     * while remaining high-level enough that we can
     * perform analysis and reasoning about the semantics
     * such as implementing a borrow checker.
     *
     * The MIR consists of two primary components.  The
     * first are the Types which represent logical layouts
     * of data.  The second are Functions which represent
     * operations that manipulate data (in types).
     *
     * The purpose of the MIR is to provide a consistent
     * view of the intermediate representation of each
     * of these things together as a unit because
     * many operations like code-generation and analysis
     * rely on them being paired and consistent.
     */
    class MIR {
    public:
	MIR();
	~MIR();
	const Types & get_types() const;
	Types & get_types();

	const Symbols & get_symbols() const;
	Symbols & get_symbols();
	
	const Functions & get_functions() const;
	Functions & get_functions();
    private:
	Functions functions;
	Types types;
	Symbols symbols;
    };

#if 0
    /**
     * This class provides the interface to create
     * new operations in a function.  Adding new operations
     * to functions should never be done outside of this
     * class because most operands require validation
     * in order to produce sensible code, so this
     * provides appropriate validations for each
     * of the opcodes and ensures that they are added
     * to the appropriate basic block only after being
     * validated.  Most validations are recoverable
     * meaning that further expression evaluation can
     * take place.  Some validations, however, cannot
     * be recovered because when determining the 'result'
     * type of the expression, something went wrong (e.g. unknown function)
     * and in those cases, it is not safe to continue
     * adding operations *in that expression*.
     */
    class MIROperationBuilder {
    public:
	MIROperationBuilder(
	    const CompilerContext & _compiler_context,
	    const MIR & _mir,
	    Function & _function,
	    size_t & _current_block
	    );
	~MIROperationBuilder();

	// Unary operations.
	bool add_operation(
	    Operation::OperationType _type,
	    JLang::context::SourceReference & _src_ref,
	    size_t & returned_tmpvar,
	    size_t & operand_tmpvar
	    );
	
	// Binary operations.
	bool add_operation(
	    Operation::OperationType _type,
	    JLang::context::SourceReference & _src_ref,
	    size_t & returned_tmpvar,
	    size_t & operand_tmpvar
	    );

	// Other operations with possibly
	// more complicated logic (function calls, literals, etc).
	bool add_operation(
	    JLang::owned<Operation> _operation
	    );
	
    private:
	const CompilerContext & compiler_context;
	const MIR & mir;
	Function & function;
	size_t & current_block;
    };
#endif
    
};
