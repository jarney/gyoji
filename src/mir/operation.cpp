#include <jlang-mir/operations.hpp>
#include <jlang-misc/jstring.hpp>

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

    // Variable access
	    
    op_type_names.insert(std::pair(Operation::OP_LOCAL_DECLARE, "declare"));
    op_type_names.insert(std::pair(Operation::OP_LOCAL_UNDECLARE, "undeclare"));
    op_type_names.insert(std::pair(Operation::OP_LOCAL_VARIABLE, "load"));
		
    // Literals
    op_type_names.insert(std::pair(Operation::OP_LITERAL_CHAR, "literal-char"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_STRING, "literal-string"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_INT, "literal-int"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_FLOAT, "literal-float"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_BOOL, "literal-bool"));
    op_type_names.insert(std::pair(Operation::OP_LITERAL_NULL, "literal-null"));
    
    // Unary operations
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
    op_type_names.insert(std::pair(Operation::OP_JUMP_CONDITIONAL, "jump-conditional"));
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
bool
Operation::is_terminating() const
{
    // Identify any instructions that
    // would terminate a basic block.
    return (type == OP_JUMP) ||
	(type == OP_JUMP_CONDITIONAL) ||
	(type == OP_RETURN);
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
    : OperationUnary(OP_DOT, _src_ref, _result, _operand)
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
    char _literal_char
    )
    : Operation(OP_LITERAL_CHAR, _src_ref, _result)
    , literal_char(_literal_char)
{}
OperationLiteralChar::~OperationLiteralChar()
{}

char
OperationLiteralChar::get_literal_char() const
{ return literal_char; }

std::string
OperationLiteralChar::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;
    std::string char_unescaped;
    char_unescaped.push_back(literal_char);
    std::string char_escaped;
    JLang::misc::string_c_escape(char_escaped, char_unescaped, true);
    
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

    std::string literal_escaped;
    JLang::misc::string_c_escape(literal_escaped, literal_string, false);

    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" \"") + literal_escaped;
    desc = desc + std::string(" \")");
    return desc;
}

//////////////////////////////////////////////
// OperationLiteralInt
//////////////////////////////////////////////
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    Type::TypeType _literal_type,
    unsigned char _literal_u8
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_type(_literal_type)
    , literal_u8(_literal_u8)
    , literal_u16(0)
    , literal_u32(0)
    , literal_u64(0L)
    , literal_i8(0)
    , literal_i16(0)
    , literal_i32(0)
    , literal_i64(0)
{}
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    Type::TypeType _literal_type,
    unsigned short _literal_u16
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_type(_literal_type)
    , literal_u8(0)
    , literal_u16(_literal_u16)
    , literal_u32(0)
    , literal_u64(0L)
    , literal_i8(0)
    , literal_i16(0)
    , literal_i32(0)
    , literal_i64(0)
{}
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    Type::TypeType _literal_type,
    unsigned int _literal_u32
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_type(_literal_type)
    , literal_u8(0)
    , literal_u16(0)
    , literal_u32(_literal_u32)
    , literal_u64(0L)
    , literal_i8(0)
    , literal_i16(0)
    , literal_i32(0)
    , literal_i64(0)
{}
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    Type::TypeType _literal_type,
    unsigned long _literal_u64
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_type(_literal_type)
    , literal_u8(0)
    , literal_u16(0)
    , literal_u32(0)
    , literal_u64(_literal_u64)
    , literal_i8(0)
    , literal_i16(0)
    , literal_i32(0)
    , literal_i64(0)
{}
/////
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    Type::TypeType _literal_type,
    char _literal_i8
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_type(_literal_type)
    , literal_u8(0)
    , literal_u16(0)
    , literal_u32(0)
    , literal_u64(0L)
    , literal_i8(_literal_i8)
    , literal_i16(0)
    , literal_i32(0)
    , literal_i64(0)
{}
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    Type::TypeType _literal_type,
    short _literal_i16
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_type(_literal_type)
    , literal_u8(0)
    , literal_u16(0)
    , literal_u32(0)
    , literal_u64(0L)
    , literal_i8(0)
    , literal_i16(_literal_i16)
    , literal_i32(0)
    , literal_i64(0)
{}
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    Type::TypeType _literal_type,
    int _literal_i32
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_type(_literal_type)
    , literal_u8(0)
    , literal_u16(0)
    , literal_u32(0)
    , literal_u64(0L)
    , literal_i8(0)
    , literal_i16(0)
    , literal_i32(_literal_i32)
    , literal_i64(0)
{}
OperationLiteralInt::OperationLiteralInt(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    Type::TypeType _literal_type,
    long _literal_i64
    )
    : Operation(OP_LITERAL_INT, _src_ref, _result)
    , literal_type(_literal_type)
    , literal_u8(0)
    , literal_u16(0)
    , literal_u32(0)
    , literal_u64(0)
    , literal_i8(0)
    , literal_i16(0)
    , literal_i32(0)
    , literal_i64(_literal_i64)
{}

OperationLiteralInt::~OperationLiteralInt()
{}

Type::TypeType
OperationLiteralInt::get_literal_type() const
{ return literal_type; }
unsigned char
OperationLiteralInt::get_literal_u8() const
{ return literal_u8; }
unsigned short
OperationLiteralInt::get_literal_u16() const
{ return literal_u16; }
unsigned int
OperationLiteralInt::get_literal_u32() const
{ return literal_u32; }
unsigned long
OperationLiteralInt::get_literal_u64() const
{ return literal_u64; }
char
OperationLiteralInt::get_literal_i8() const
{ return literal_i8; }
short
OperationLiteralInt::get_literal_i16() const
{ return literal_i16; }
int
OperationLiteralInt::get_literal_i32() const
{ return literal_i32; }
long
OperationLiteralInt::get_literal_i64() const
{ return literal_i64; }

std::string
OperationLiteralInt::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string literalstr;
    std::string literaltype;
    
    switch (literal_type) {
    case Type::TYPE_PRIMITIVE_u8:
	literalstr = std::to_string(literal_u8);
	literaltype = "u8";
	break;
    case Type::TYPE_PRIMITIVE_u16:
	literalstr = std::to_string(literal_u16);
	literaltype = "u16";
	break;
    case Type::TYPE_PRIMITIVE_u32:
	literalstr = std::to_string(literal_u32);
	literaltype = "u32";
	break;
    case Type::TYPE_PRIMITIVE_u64:
	literalstr = std::to_string(literal_u64);
	literaltype = "u64";
	break;
    case Type::TYPE_PRIMITIVE_i8:
	literalstr = std::to_string(literal_i8);
	literaltype = "i8";
	break;
    case Type::TYPE_PRIMITIVE_i16:
	literalstr = std::to_string(literal_i16);
	literaltype = "i16";
	break;
    case Type::TYPE_PRIMITIVE_i32:
	literalstr = std::to_string(literal_i32);
	literaltype = "i32";
	break;
    case Type::TYPE_PRIMITIVE_i64:
	literalstr = std::to_string(literal_i64);
	literaltype = "i64";
	break;
    default:
	literalstr = "Error: Unsupported literal int type";
	break;
    }
    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + literalstr + literaltype;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationLiteralFloat
//////////////////////////////////////////////
	    
OperationLiteralFloat::OperationLiteralFloat(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    float _literal_float
    )
    : Operation(OP_LITERAL_FLOAT, _src_ref, _result)
    , literal_type(Type::TYPE_PRIMITIVE_f32)
    , literal_float_f32(_literal_float)
    , literal_float_f64(0.0)
{}
OperationLiteralFloat::OperationLiteralFloat(
    const JLang::context::SourceReference & _src_ref,
    size_t _result,
    double _literal_float
    )
    : Operation(OP_LITERAL_FLOAT, _src_ref, _result)
    , literal_type(Type::TYPE_PRIMITIVE_f64)
    , literal_float_f32(0.0)
    , literal_float_f64(_literal_float)
{}
OperationLiteralFloat::~OperationLiteralFloat()
{}

float
OperationLiteralFloat::get_literal_float() const
{ return literal_float_f32; }

double
OperationLiteralFloat::get_literal_double() const
{ return literal_float_f64; }

Type::TypeType
OperationLiteralFloat::get_literal_type() const
{ return literal_type; }
std::string
OperationLiteralFloat::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string literal_float;
    switch (literal_type) {
    case Type::TYPE_PRIMITIVE_f32:
	literal_float = std::to_string(literal_float_f32) + std::string(" f32");
	break;
    case Type::TYPE_PRIMITIVE_f64:
	literal_float = std::to_string(literal_float_f64) + std::string(" f64");
	break;
    default:
	fprintf(stderr, "Compiler Bug! Unknown literal float type\n");
	exit(1);
    }
    
    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + literal_float;
    desc = desc + std::string(" )");
    return desc;
}

//////////////////////////////////////////////
// OperationLiteralBool
//////////////////////////////////////////////
OperationLiteralBool::OperationLiteralBool(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    bool _literal_bool
	    )
    : Operation(OP_LITERAL_BOOL, _src_ref, _result)
    , literal_bool(_literal_bool)
{}
OperationLiteralBool::~OperationLiteralBool()
{}
bool
OperationLiteralBool::get_literal_bool() const
{ return literal_bool; }
std::string
OperationLiteralBool::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;
    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" (");
    desc = desc + std::string(" ") + (literal_bool ? std::string("true") : std::string("false"));
    desc = desc + std::string(" )");
    return desc;
}
//////////////////////////////////////////////
// OperationLiteralNull
//////////////////////////////////////////////
OperationLiteralNull::OperationLiteralNull(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result
	    )
    : Operation(OP_LITERAL_NULL, _src_ref, _result)
{}
OperationLiteralNull::~OperationLiteralNull()
{}
std::string
OperationLiteralNull::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;
    std::string desc = std::string("_") + std::to_string(result) + std::string(" = ") + op_name + std::string(" ( )");
    return desc;
}


//////////////////////////////////////////////
// OperationJumpConditional
//////////////////////////////////////////////
OperationJumpConditional::OperationJumpConditional(
    const JLang::context::SourceReference & _src_ref,
    size_t _operand,
    size_t _if_block,
    size_t _else_block
    )
    : Operation(OP_JUMP_CONDITIONAL, _src_ref, 0, _operand, _if_block, _else_block)
{}

OperationJumpConditional::~OperationJumpConditional()
{}
    
std::string
OperationJumpConditional::get_description() const
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
    const Type *_variable_type
    )
    : Operation(OP_LOCAL_DECLARE, _src_ref, 0)
    , variable(_variable)
    , variable_type(_variable_type)
{}
OperationLocalDeclare::~OperationLocalDeclare()
{}
const std::string &
OperationLocalDeclare::get_variable() const
{ return variable; }
const Type*
OperationLocalDeclare::get_variable_type() const
{ return variable_type; }

std::string
OperationLocalDeclare::get_description() const
{
    const auto & it = op_type_names.find(type);
    const std::string & op_name = it->second;

    std::string desc = op_name + std::string(" (");
    desc = desc + std::string(" ") + variable;
    desc = desc + std::string(" ") + variable_type->get_name();
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

