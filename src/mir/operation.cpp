#include <jlang-mir/operations.hpp>

using namespace JLang::mir;

namespace JLang::mir {
    static std::map<Operation::OperationType, std::string> op_type_names;
}

void JLang::mir::operation_static_init()
{
    // This doesn't really belong here, but I don't have a
    // better place to globally initialize static data.


    // Functions and global symbols
    op_type_names.insert(std::pair(Operation::OP_FUNCTION_CALL, "function-call"));
    op_type_names.insert(std::pair(Operation::OP_SYMBOL, "symbol"));
    
    // Cast operations.
    op_type_names.insert(std::pair(Operation::OP_WIDEN_SIGNED, "widen-signed"));
    op_type_names.insert(std::pair(Operation::OP_WIDEN_UNSIGNED, "widen-unsigned"));
    op_type_names.insert(std::pair(Operation::OP_WIDEN_FLOAT, "widen-float"));
    
    // Indirect access
    op_type_names.insert(std::pair(Operation::OP_ARRAY_INDEX, "array-index"));
    op_type_names.insert(std::pair(Operation::OP_DOT, "dot"));
    op_type_names.insert(std::pair(Operation::OP_ARROW, "arrow"));

    // Variable access
	    
    op_type_names.insert(std::pair(Operation::OP_LOCAL_DECLARE, "declare"));
    op_type_names.insert(std::pair(Operation::OP_LOCAL_UNDECLARE, "undeclare"));
    op_type_names.insert(std::pair(Operation::OP_LOCAL_VARIABLE, "load"));
		
    // Literals
    op_type_names.insert(std::pair(Operation::OP_LITERAL_CHAR, "literal-char"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_STRING, "literal-string"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_INT, "literal-int"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_FLOAT, "literal-float"));
    
    // Unary operations
    op_type_names.insert(std::pair(Operation::OP_POST_INCREMENT, "post-increment"));
    op_type_names.insert(std::pair(Operation::OP_POST_DECREMENT, "post-decrement"));
    op_type_names.insert(std::pair(Operation::OP_PRE_INCREMENT, "pre-increment"));
    op_type_names.insert(std::pair(Operation::OP_PRE_INCREMENT, "pre-decrement"));
    op_type_names.insert(std::pair(Operation::OP_ADDRESSOF, "addressof"));
    op_type_names.insert(std::pair(Operation::OP_DEREFERENCE, "dereference"));
    op_type_names.insert(std::pair(Operation::OP_NEGATE, "negate"));
    op_type_names.insert(std::pair(Operation::OP_BITWISE_NOT, "bitwise-not"));
    op_type_names.insert(std::pair(Operation::OP_LOGICAL_NOT, "logical-not"));
    op_type_names.insert(std::pair(Operation::OP_SIZEOF_TYPE, "sizeof"));

    // Binary operations: arithmetic
    op_type_names.insert(std::pair(Operation::OP_ADD, "add"));
    op_type_names.insert(std::pair(Operation::OP_SUBTRACT, "subtract"));
    op_type_names.insert(std::pair(Operation::OP_MULTIPLY, "multiply"));
    op_type_names.insert(std::pair(Operation::OP_DIVIDE, "divide"));
    op_type_names.insert(std::pair(Operation::OP_MODULO, "modulo"));
	    
    // Binary operations: logical
    op_type_names.insert(std::pair(Operation::OP_LOGICAL_AND, "logical-and"));
    op_type_names.insert(std::pair(Operation::OP_LOGICAL_OR, "logical-or"));

    // Binary operations: bitwise
    op_type_names.insert(std::pair(Operation::OP_BITWISE_AND, "bitwise-and"));
    op_type_names.insert(std::pair(Operation::OP_BITWISE_OR, "bitwise-or"));
    op_type_names.insert(std::pair(Operation::OP_BITWISE_XOR, "bitwise-xor"));
    op_type_names.insert(std::pair(Operation::OP_SHIFT_LEFT, "bitwise-shift-left"));
    op_type_names.insert(std::pair(Operation::OP_SHIFT_RIGHT, "bitwise-shift-right"));

    // Binary operations: comparisons
    op_type_names.insert(std::pair(Operation::OP_COMPARE_LT, "compare-lt"));
    op_type_names.insert(std::pair(Operation::OP_COMPARE_GT, "compare-gt"));
    op_type_names.insert(std::pair(Operation::OP_COMPARE_LE, "compare-le"));
    op_type_names.insert(std::pair(Operation::OP_COMPARE_GE, "compare-ge"));
    op_type_names.insert(std::pair(Operation::OP_COMPARE_NE, "compare-ne"));
    op_type_names.insert(std::pair(Operation::OP_COMPARE_EQ, "compare-eq"));

    // Binary operations: assignment
    op_type_names.insert(std::pair(Operation::OP_ASSIGN, "store"));
	    
    // Branch and flow control
    op_type_names.insert(std::pair(Operation::OP_JUMP_IF_EQUAL, "jump-if"));
    op_type_names.insert(std::pair(Operation::OP_JUMP, "jump"));
    op_type_names.insert(std::pair(Operation::OP_RETURN, "return"));
}

Operation::Operation(
    OperationType _type,
    const JLang::context::SourceReference & _src_ref,
    size_t _result
    )
    : type(_type)
    , src_ref(_src_ref)
    , result(_result)
{}
Operation::Operation(
    OperationType _type,
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _operand
    )
    : type(_type)
    , src_ref(_src_ref)
    , result(_result)
{
    add_operand(_operand);
}
Operation::Operation(
    OperationType _type,
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _operand_a,
    size_t _operand_b
    )
    : type(_type)
    , src_ref(_src_ref)
    , result(_result)
{
    add_operand(_operand_a);
    add_operand(_operand_b);
}
Operation::Operation(
    OperationType _type,
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _operand_a,
    size_t _operand_b,
    size_t _operand_c
    )
    : type(_type)
    , src_ref(_src_ref)
    , result(_result)
{
    add_operand(_operand_a);
    add_operand(_operand_b);
    add_operand(_operand_c);
}
Operation::~Operation()
{}

void
Operation::add_operand(size_t operand)
{
    operands.push_back(operand);
}

std::string
Operation::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    for (const auto & operand : operands) {
	desc = desc + std::string(" _") + std::to_string(operand);
    }
    desc = desc + std::string(" )");
    
    return desc;
}

Operation::OperationType
Operation::get_type() const
{ return type; }

const std::vector<size_t> &
Operation::get_operands() const
{ return operands; }

size_t
Operation::get_result() const
{ return result; }

const JLang::context::SourceReference &
Operation::get_source_ref() const
{ return src_ref; }

void
Operation::dump(FILE *out) const
{
    fprintf(out, "            %s\n", get_description().c_str());
}


//////////////////////////////////////////////
// OperationUnary
//////////////////////////////////////////////
OperationUnary::OperationUnary(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    )
    : Operation(_type, _src_ref, _result, _operand)
{}
OperationUnary::~OperationUnary()
{}
size_t
OperationUnary::get_a() const
{ return operands.at(0); }
//////////////////////////////////////////////
// OperationCast
//////////////////////////////////////////////
OperationCast::OperationCast(
    OperationType _type,
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _operand,
    const Type *_cast_type
    )
    : OperationUnary(_type, _src_ref, _result, _operand)
    , cast_type(_cast_type)
{}
OperationCast::~OperationCast()
{}

const Type*
OperationCast::get_cast_type() const
{ return cast_type; }

std::string
OperationCast::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" _") + std::to_string(operands.at(0));
    desc = desc + std::string(" ") + cast_type->get_name();
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationBinary
//////////////////////////////////////////////

OperationBinary::OperationBinary(
    OperationType _type,
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _operand_a,
    size_t _operand_b
	    )
    : Operation(_type, _src_ref, _result, _operand_a, _operand_b)
{}
OperationBinary::~OperationBinary()
{}
size_t
OperationBinary::get_a() const
{ return operands.at(0); }

size_t
OperationBinary::get_b() const
{ return operands.at(1); }


//////////////////////////////////////////////
// OperationFunctionCall
//////////////////////////////////////////////
OperationFunctionCall::OperationFunctionCall(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _callee_tmpvar,
    std::vector<size_t> _arg_args
    )
    : Operation(OP_FUNCTION_CALL, _src_ref, _result, _callee_tmpvar)
{
    for (const auto & av : _arg_args) {
	add_operand(av);
    }
}

OperationFunctionCall::~OperationFunctionCall()
{}

//////////////////////////////////////////////
// OperationSymbol
//////////////////////////////////////////////
OperationSymbol::OperationSymbol(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    std::string _symbol_name
    )
    : Operation(OP_SYMBOL, _src_ref, _result)
    , symbol_name(_symbol_name)
{}

OperationSymbol::~OperationSymbol()
{}

const std::string &
OperationSymbol::get_symbol_name() const
{ return symbol_name; }

std::string
OperationSymbol::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + symbol_name;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationArrayIndex
//////////////////////////////////////////////
OperationArrayIndex::OperationArrayIndex(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _array_tmpvar,
    size_t _index_tmpvar
    )
    : OperationBinary(OP_ARRAY_INDEX, _src_ref, _result, _array_tmpvar, _index_tmpvar)
{}
OperationArrayIndex::~OperationArrayIndex()
{}

//////////////////////////////////////////////
// OperationDot
//////////////////////////////////////////////
OperationDot::OperationDot(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _operand,
    std::string _member_name
    )
    : Operation(OP_DOT, _src_ref, _result, _operand)
    , member_name(_member_name)
{}

OperationDot::~OperationDot()
{}

const std::string &
OperationDot::get_member_name() const
{ return member_name; }

std::string
OperationDot::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" _") + std::to_string(operands.at(0));
    desc = desc + std::string(" ") + member_name;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationArrow
//////////////////////////////////////////////
OperationArrow::OperationArrow(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    size_t _operand,
    std::string _member_name
    )
    : Operation(OP_ARROW, _src_ref, _result)
    , member_name(_member_name)
{}

OperationArrow::~OperationArrow()
{}

const std::string &
OperationArrow::get_member_name() const
{ return member_name; }

std::string
OperationArrow::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" _") + std::to_string(operands.at(0));
    desc = desc + std::string(" ") + member_name;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationLocalVariable
//////////////////////////////////////////////
OperationLocalVariable::OperationLocalVariable(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    std::string _symbol_name,
    const Type * _var_type
    )
    : Operation(OP_LOCAL_VARIABLE, _src_ref, _result)
    , symbol_name(_symbol_name)
    , var_type(_var_type)
{}
OperationLocalVariable::~OperationLocalVariable()
{}

const std::string &
OperationLocalVariable::get_symbol_name() const
{ return symbol_name; }

const Type *
OperationLocalVariable::get_var_type() const
{ return var_type; }

std::string
OperationLocalVariable::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + symbol_name;
    desc = desc + std::string(" ") + var_type->get_name();
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationLiteralChar
//////////////////////////////////////////////
OperationLiteralChar::OperationLiteralChar(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    std::string _literal_char
    )
    : Operation(OP_LITERAL_CHAR, _src_ref, _result)
    , literal_char(_literal_char)
{}
OperationLiteralChar::~OperationLiteralChar()
{}

const std::string &
OperationLiteralChar::get_literal_char() const
{ return literal_char; }

std::string
OperationLiteralChar::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + literal_char;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationLiteralString
//////////////////////////////////////////////
OperationLiteralString::OperationLiteralString(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    std::string _literal_string
    )
    : Operation(OP_LITERAL_STRING, _src_ref, _result)
    , literal_string(_literal_string)
{}
OperationLiteralString::~OperationLiteralString()
{}

const std::string &
OperationLiteralString::get_literal_string() const
{ return literal_string; }

std::string
OperationLiteralString::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + literal_string;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationLiteralInt
//////////////////////////////////////////////
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    std::string _literal_int,
    const Type *_type
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_int(_literal_int)
    , literal_type(_type)
{}
OperationLiteralInt::~OperationLiteralInt()
{}

const std::string &
OperationLiteralInt::get_literal_int() const
{ return literal_int; }
const Type *
OperationLiteralInt::get_literal_type() const
{ return literal_type; }

std::string
OperationLiteralInt::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + literal_int;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationLiteralFloat
//////////////////////////////////////////////
OperationLiteralFloat::OperationLiteralFloat(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    std::string _literal_float
    )
    : Operation(OP_LITERAL_FLOAT, _src_ref, _result)
    , literal_float(_literal_float)
{}
OperationLiteralFloat::~OperationLiteralFloat()
{}

const std::string &
OperationLiteralFloat::get_literal_float() const
{ return literal_float; }

std::string
OperationLiteralFloat::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + literal_float;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationJumpIfEqual
//////////////////////////////////////////////
OperationJumpIfEqual::OperationJumpIfEqual(
    const JLang::context::SourceReference & _src_ref,
    size_t _operand,
    size_t _if_block,
    size_t _else_block
    )
    : Operation(OP_JUMP_IF_EQUAL, _src_ref, 0, _operand, _if_block, _else_block)
{}

OperationJumpIfEqual::~OperationJumpIfEqual()
{}
    
std::string
OperationJumpIfEqual::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = op_name + std::string(" (");
    desc = desc + std::string(" _") + std::to_string(operands.at(0));
    desc = desc + std::string(" BB") + std::to_string(operands.at(1));
    desc = desc + std::string(" BB") + std::to_string(operands.at(2));
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationJump
//////////////////////////////////////////////
OperationJump::OperationJump(
    const JLang::context::SourceReference & _src_ref,
    size_t _block
    )
    : Operation(OP_JUMP, _src_ref, 0, _block)
{}
OperationJump::~OperationJump()
{}

std::string
OperationJump::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = op_name + std::string(" (");
    desc = desc + std::string(" BB") + std::to_string(operands.at(0));
    desc = desc + std::string(" )");
    return desc;
}
    
//////////////////////////////////////////////
// OperationReturn
//////////////////////////////////////////////
OperationReturn::OperationReturn(
    const JLang::context::SourceReference & _src_ref,
    size_t _operand
    )
    : Operation(OP_RETURN, _src_ref, 0, _operand)
{}
OperationReturn::~OperationReturn()
{}

std::string
OperationReturn::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = op_name + std::string(" (");
    desc = desc + std::string(" _") + std::to_string(operands.at(0));
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationLocalDeclare
//////////////////////////////////////////////
OperationLocalDeclare::OperationLocalDeclare(
    const JLang::context::SourceReference & _src_ref,
    std::string _variable,
    std::string _var_type
    )
    : Operation(OP_LOCAL_DECLARE, _src_ref, 0)
    , variable(_variable)
    , var_type(_var_type)
{}
OperationLocalDeclare::~OperationLocalDeclare()
{}
const std::string &
OperationLocalDeclare::get_variable() const
{ return variable; }
const std::string &
OperationLocalDeclare::get_var_type() const
{ return var_type; }

std::string
OperationLocalDeclare::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = op_name + std::string(" (");
    desc = desc + std::string(" ") + variable;
    desc = desc + std::string(" ") + var_type;
    desc = desc + std::string(" )");
    return desc;
}
//////////////////////////////////////////////
// OperationLocalUndeclare
//////////////////////////////////////////////
OperationLocalUndeclare::OperationLocalUndeclare(
    const JLang::context::SourceReference & _src_ref,
    std::string _variable
    )
    : Operation(OP_LOCAL_UNDECLARE, _src_ref, 0)
    , variable(_variable)
{}

OperationLocalUndeclare::~OperationLocalUndeclare()
{}

std::string
OperationLocalUndeclare::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = op_name + std::string(" (");
    desc = desc + std::string(" ") + variable;
    desc = desc + std::string(" )");
    return desc;
}
//////////////////////////////////////////////
// OperationSizeofType
//////////////////////////////////////////////
OperationSizeofType::OperationSizeofType(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    const Type *_type
    )
    : Operation(OP_SIZEOF_TYPE, _src_ref, _result)
    , type(_type)
{}
OperationSizeofType::~OperationSizeofType()
{}

const Type *
OperationSizeofType::get_type() const
{ return type; }

