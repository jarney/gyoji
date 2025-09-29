#include <jlang-mir.hpp>

using namespace JLang::mir;

MIR::MIR()
{}

MIR::~MIR()
{}

const Types &
MIR::get_types() const
{ return types; }

Types &
MIR::get_types()
{ return types; }

const Functions &
MIR::get_functions() const
{ return functions; }

Functions &
MIR::get_functions()
{ return functions; }

Symbols &
MIR::get_symbols()
{ return symbols; }

const Symbols &
MIR::get_symbols() const
{ return symbols; }


#if 0
//////////////////////////////////////////////
// MIROperationBuilder
//////////////////////////////////////////////
MIROperationBuilder::MIROperationBuilder(
	    const CompilerContext & _compiler_context,
	    const MIR & _mir,
	    Function & _function,
	    size_t & _current_block
	    )
{
    validation_table[Operation::OP_ADD] = op_add_validator;
}

MIROperationBuilder::~MIROperationBuilder();

// Unary operations.
bool
MIROperationBuilder::add_operation(
    Operation::OperationType _type,
    JLang::context::SourceReference & _src_ref,
    size_t & returned_tmpvar,
    size_t & operand_tmpvar
    )
{}
// Binary operations.
bool
MIROperationBuilder::add_operation(
    Operation::OperationType _type,
    JLang::context::SourceReference & _src_ref,
    size_t & _returned_tmpvar,
    size_t & _a_tmpvar
    size_t & _b_tmpvar
    )
{
    // Validate
    auto validator = validation_table[_type];

    // Validate the input operands
    // and finalize the return-value.
    bool rc = validator.validate(
	returned_type,
	_a_tmpvar,
	_b_tmpvar
	);
    if (!rc) return rc;

    // Figure out how to determine the return type
    // based on the operands.
    
    _returned_tmpvar = function.tmpvar_duplicate(_a_tmpvar);
    auto operation = std::make_unique<OperationBinary>(
	_type,
	_src_ref,
	_returned_tmpvar,
	_a_tmpvar,
	_b_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
    return true;
}


// Other operations with possibly
// more complicated logic (function calls, literals, etc).
bool
MIROperationBuilder::add_operation(
    JLang::owned<Operation> _operation
    )
{ return true; }

#endif

