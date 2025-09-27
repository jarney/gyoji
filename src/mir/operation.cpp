#include <jlang-mir/operations.hpp>

using namespace JLang::mir;

namespace JLang::mir {
    static std::map<Operation::OperationType, std::string> op_type_names;
}

void JLang::mir::operation_static_init()
{
    // This doesn't really belong here, but I don't have a
    // better place to globally initialize static data.
    op_type_names.insert(std::pair(Operation::OP_FUNCTION_CALL, "function-call"));
    op_type_names.insert(std::pair(Operation::OP_SYMBOL, "symbol"));
    op_type_names.insert(std::pair(Operation::OP_LOCAL_DECLARE, "declare"));
    op_type_names.insert(std::pair(Operation::OP_LOCAL_UNDECLARE, "undeclare"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_CHAR, "literal-char"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_STRING, "literal-string"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_INT, "literal-int"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_FLOAT, "literal-float"));
    op_type_names.insert(std::pair(Operation::OP_PRE_INCREMENT, "prefix++"));
    op_type_names.insert(std::pair(Operation::OP_POST_INCREMENT, "postfix++"));
    op_type_names.insert(std::pair(Operation::OP_PRE_DECREMENT, "prefix--"));
    op_type_names.insert(std::pair(Operation::OP_POST_DECREMENT, "postfix--"));
    op_type_names.insert(std::pair(Operation::OP_ADD, "add"));
    op_type_names.insert(std::pair(Operation::OP_SUBTRACT, "subtract"));
    op_type_names.insert(std::pair(Operation::OP_MULTIPLY, "multiply"));
    op_type_names.insert(std::pair(Operation::OP_DIVIDE, "divide"));
    op_type_names.insert(std::pair(Operation::OP_ASSIGN, "assign"));
    op_type_names.insert(std::pair(Operation::OP_JUMP_IF_EQUAL, "je"));
    op_type_names.insert(std::pair(Operation::OP_JUMP, "jmp"));
    op_type_names.insert(std::pair(Operation::OP_RETURN, "return"));
}




Operation::Operation(OperationType _type, size_t _result)
    : type(_type)
    , result(_result)
{}
Operation::Operation(OperationType _type, size_t _result, size_t _operand)
    : type(_type)
    , result(_result)
{
    add_operand(_operand);
}
Operation::Operation(OperationType _type, size_t _result, size_t _operand_a, size_t _operand_b)
    : type(_type)
    , result(_result)
{
    add_operand(_operand_a);
    add_operand(_operand_b);
}
Operation::~Operation()
{}

Operation::Operation(const Operation & _other)
    : type(_other.type)
    , operands(_other.operands)
    , result(_other.result)
{}

void
Operation::add_operand(size_t operand)
{
    operands.push_back(operand);
}

std::string
Operation::get_name() const
{
    const auto & it = op_type_names.find(type);
    return it->second;
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

void
Operation::dump() const
{
    std::string name = get_name();
    fprintf(stderr, "_%ld = %s (", result, name.c_str());
    for (const auto & operand : operands) {
	fprintf(stderr, " %ld", operand);
    }
    fprintf(stderr, " )\n");
}


//////////////////////////////////////////////
// OperationFunctionCall
//////////////////////////////////////////////
OperationFunctionCall::OperationFunctionCall(size_t _result)
    : Operation(OP_FUNCTION_CALL, _result)
{}

OperationFunctionCall::~OperationFunctionCall()
{}

//////////////////////////////////////////////
// OperationSymbol
//////////////////////////////////////////////
OperationSymbol::OperationSymbol(size_t _result, std::string _symbol_name)
    : Operation(OP_SYMBOL, _result)
    , symbol_name(_symbol_name)
{}

OperationSymbol::~OperationSymbol()
{}

const std::string &
OperationSymbol::get_symbol_name() const
{ return symbol_name; }


//////////////////////////////////////////////
// OperationLiteralChar
//////////////////////////////////////////////
OperationLiteralChar::OperationLiteralChar(size_t _result, std::string _literal_char)
    : Operation(OP_LITERAL_CHAR, _result)
    , literal_char(_literal_char)
{}
OperationLiteralChar::~OperationLiteralChar()
{}

const std::string &
OperationLiteralChar::get_literal_char()
{ return literal_char; }

//////////////////////////////////////////////
// OperationLiteralString
//////////////////////////////////////////////
OperationLiteralString::OperationLiteralString(size_t _result, std::string _literal_string)
    : Operation(OP_LITERAL_STRING, _result)
    , literal_string(_literal_string)
{}
OperationLiteralString::~OperationLiteralString()
{}

const std::string &
OperationLiteralString::get_literal_string()
{ return literal_string; }

//////////////////////////////////////////////
// OperationLiteralInt
//////////////////////////////////////////////
OperationLiteralInt::OperationLiteralInt(size_t _result, std::string _literal_int)
    : Operation(OP_LITERAL_INT, _result)
    , literal_int(_literal_int)
{}
OperationLiteralInt::~OperationLiteralInt()
{}

const std::string &
OperationLiteralInt::get_literal_int()
{ return literal_int; }

//////////////////////////////////////////////
// OperationLiteralFloat
//////////////////////////////////////////////
OperationLiteralFloat::OperationLiteralFloat(size_t _result, std::string _literal_float)
    : Operation(OP_LITERAL_FLOAT, _result)
    , literal_float(_literal_float)
{}
OperationLiteralFloat::~OperationLiteralFloat()
{}

const std::string &
OperationLiteralFloat::get_literal_float()
{ return literal_float; }

//////////////////////////////////////////////
// OperationPreIncrement
//////////////////////////////////////////////
OperationPreIncrement::OperationPreIncrement(size_t _result, size_t _operand)
    : Operation(OP_PRE_INCREMENT, _result, _operand)
{}
OperationPreIncrement::~OperationPreIncrement()
{}

//////////////////////////////////////////////
// OperationPostIncrement
//////////////////////////////////////////////
OperationPostIncrement::OperationPostIncrement(size_t _result, size_t _operand)
    : Operation(OP_PRE_INCREMENT, _result, _operand)
{}

OperationPostIncrement::~OperationPostIncrement()
{}

//////////////////////////////////////////////
// OperationPreDecrement
//////////////////////////////////////////////
OperationPreDecrement::OperationPreDecrement(size_t _result, size_t _operand)
    : Operation(OP_PRE_INCREMENT, _result, _operand)
{}
OperationPreDecrement::~OperationPreDecrement()
{}

//////////////////////////////////////////////
// OperationPostDecrement
//////////////////////////////////////////////
OperationPostDecrement::OperationPostDecrement(size_t _result, size_t _operand)
    : Operation(OP_PRE_INCREMENT, _result, _operand)
{}

OperationPostDecrement::~OperationPostDecrement()
{}


//////////////////////////////////////////////
// OperationJumpIfEqual
//////////////////////////////////////////////
OperationJumpIfEqual::OperationJumpIfEqual(size_t _operand, std::string _label)
    : Operation(OP_JUMP_IF_EQUAL, 0, _operand)
    , label(_label)
{}

OperationJumpIfEqual::~OperationJumpIfEqual()
{}
    
//////////////////////////////////////////////
// OperationJump
//////////////////////////////////////////////
OperationJump::OperationJump(std::string _label)
    : Operation(OP_JUMP, 0)
    , label(_label)
{}
OperationJump::~OperationJump()
{}
    
//////////////////////////////////////////////
// OperationReturn
//////////////////////////////////////////////
OperationReturn::OperationReturn(size_t _operand)
    : Operation(OP_JUMP, 0, _operand)
{}
OperationReturn::~OperationReturn()
{}

//////////////////////////////////////////////
// OperationLocalDeclare
//////////////////////////////////////////////
OperationLocalDeclare::OperationLocalDeclare(std::string _variable)
    : Operation(OP_LOCAL_DECLARE, 0)
    , variable(_variable)
{}
OperationLocalDeclare::~OperationLocalDeclare()
{}

//////////////////////////////////////////////
// OperationLocalUndeclare
//////////////////////////////////////////////
OperationLocalUndeclare::OperationLocalUndeclare(std::string _variable)
    : Operation(OP_LOCAL_UNDECLARE, 0)
    , variable(_variable)
{}

OperationLocalUndeclare::~OperationLocalUndeclare()
{}

