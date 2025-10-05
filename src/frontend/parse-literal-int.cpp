#include <jlang-frontend.hpp>
#include <jlang-misc/jstring.hpp>

using namespace JLang::frontend::integers;
using namespace JLang::mir;

static std::string u8_type("u8");
static std::string u16_type("u16");
static std::string u32_type("u32");
static std::string u64_type("u64");
    
static std::string i8_type("i8");
static std::string i16_type("i16");
static std::string i32_type("i32");
static std::string i64_type("i64");

bool JLang::frontend::integers::parse_literal_int(
    const JLang::context::CompilerContext & compiler_context,
    const JLang::mir::Types & types,
    const JLang::frontend::tree::Terminal & literal_int_token,
    ParseLiteralIntResult & result
    )
{
    size_t radix = 10;
    // Extremely ineficcient, but we need to resolve
    // the specific type of integer here
    // so that the MIR layer knows what we mean.
    // Actually, the whole of this logic
    // should probably move down one notch to the
    // construction OP_LITERAL in the FunctionResolver
    result.parsed_type = nullptr;

    std::string integer_part;
    Type* type_part = nullptr;

    bool sign_positive = true;

    const std::string & token_value = literal_int_token.get_value();
    size_t len = token_value.size();
    if (JLang::misc::endswith(token_value, u8_type)) {
	integer_part = token_value.substr(0, len - u8_type.size());
	type_part = types.get_type("u8");
    }
    else if (JLang::misc::endswith(token_value, u16_type)) {
	integer_part = token_value.substr(0, len - u16_type.size());
	type_part = types.get_type("u16");
    }
    else if (JLang::misc::endswith(token_value, u32_type)) {
	integer_part = token_value.substr(0, len - u32_type.size());
	type_part = types.get_type("u32");
    }
    else if (JLang::misc::endswith(token_value, u64_type)) {
	integer_part = token_value.substr(0, len - u64_type.size());
	type_part = types.get_type("u64");
    }
    else if (JLang::misc::endswith(token_value, i8_type)) {
	integer_part = token_value.substr(0, len - i8_type.size());
	type_part = types.get_type("i8");
    }
    else if (JLang::misc::endswith(token_value, i16_type)) {
	integer_part = token_value.substr(0, len - i16_type.size());
	type_part = types.get_type("i16");
    }
    else if (JLang::misc::endswith(token_value, i32_type)) {
	integer_part = token_value.substr(0, len - i32_type.size());
	type_part = types.get_type("i32");
    }
    else if (JLang::misc::endswith(token_value, i64_type)) {
	integer_part = token_value.substr(0, len - i64_type.size());
	type_part = types.get_type("i64");
    }
    else {
	// If not specified, we assume u32 or i32 depending
	// on whether there's a '-' at the start.
	integer_part = token_value;
	if (integer_part[0] == '-') {
	    type_part = types.get_type("i32");
	    sign_positive = false;
	}
	else {
	    type_part = types.get_type("u32");
	    sign_positive = true;
	}
    }
    
    // If the remaining part of the number
    // starts with a '-' then we flip the sign bit
    // to make it negative and remove the '-' so
    // we can parse the radix part.
    if (JLang::misc::startswith(integer_part, "-")) {
	integer_part = integer_part.substr(1);
	sign_positive = false;
    }
    else {
	sign_positive = true;
    }

    // Next, we need to check that
    // if it's a negative number, it is
    // consistent with the signedness of the type.
    if (!sign_positive && type_part->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		literal_int_token.get_source_ref(),
		"Integer literal type mismatch",
		std::string("Type of integer literal was unsigned, but a negative value was given")
		);
	return false;
    }
    
    
    // Remove the superfluous '_' which are only
    // supported for readability purposes and play
    // no semantic role.
    // Hex is supported
    integer_part = JLang::misc::string_remove(integer_part, "_");
    if (JLang::misc::startswith(integer_part, "0x")) {
	integer_part = JLang::misc::string_remove(integer_part, "0x");
	radix = 16;
    }
    // Binary is supported
    else if (JLang::misc::startswith(integer_part, "0b")) {
	integer_part = JLang::misc::string_remove(integer_part, "0b");
	radix = 2;
    }
    // Octal is supported.
    else if (JLang::misc::startswith(integer_part, "0o")) {
	integer_part = JLang::misc::string_remove(integer_part, "0o");
	radix = 8;
    }
    // If radix is not specified, assume decimal.

    const char *source_cstring = integer_part.c_str();
    size_t length = integer_part.size();
    
    char *endptr = nullptr;
    errno = 0;
    
    
    switch (type_part->get_type()) {
    case Type::TYPE_PRIMITIVE_u8:
    {
	unsigned long number = strtoul(source_cstring, &endptr, radix);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value contained extraneous characters: ") + integer_part
		    );
	    return false;
	}
	if (errno == ERANGE || number >= 256) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value ") + integer_part + std::string(" is outside the range of values allowed in a u8")
		    );
	    return false;
	}
	result.parsed_type = type_part;
	result.u8_value = (unsigned char)number;
	return true;
    }
    case Type::TYPE_PRIMITIVE_u16:
    {
	unsigned long number = strtoul(source_cstring, &endptr, radix);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value contained extraneous characters: ") + integer_part
		    );
	    return false;
	}
	if (errno == ERANGE || number > 0xffff) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value ") + integer_part + std::string(" is outside the range of values allowed in a u16")
		    );
	    return false;
	}
	result.parsed_type = type_part;
	result.u16_value = (unsigned short)number;
	return true;
    }
	break;
    case Type::TYPE_PRIMITIVE_u32:
    {
	unsigned long number = strtoull(source_cstring, &endptr, radix);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value contained extraneous characters: ") + integer_part
		    );
	    return false;
	}
	if (errno == ERANGE || number > 0xffffffff) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value ") + integer_part + std::string(" is outside the range of values allowed in a u32")
		    );
	    return false;
	}
	result.parsed_type = type_part;
	result.u32_value = (unsigned int)number;
	return true;
    }
	break;
    case Type::TYPE_PRIMITIVE_u64:
    {
	unsigned long number = strtoull(source_cstring, &endptr, radix);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value contained extraneous characters: ") + integer_part
		    );
	    return false;
	}
	if (errno == ERANGE) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value ") + integer_part + std::string(" is outside the range of values allowed in a u64")
		    );
	    return false;
	}
	result.parsed_type = type_part;
	result.u64_value = (unsigned long)number;
	return true;
    }
	break;

    // Signed
    case Type::TYPE_PRIMITIVE_i8:
    {
	long number = strtol(source_cstring, &endptr, radix);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value contained extraneous characters: ") + integer_part
		    );
	    return false;
	}
	number = sign_positive ? number : -number;
	if (errno == ERANGE || number < -128 || number > 127) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value ") + integer_part + std::string(" is outside the range of values allowed in a i8")
		    );
	    return false;
	}
	result.parsed_type = type_part;
	result.i8_value = (char)number;
	return true;
    }
    case Type::TYPE_PRIMITIVE_i16:
    {
	long number = strtol(source_cstring, &endptr, radix);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value contained extraneous characters: ") + integer_part
		    );
	    return false;
	}
	number = sign_positive ? number : -number;
	if (errno == ERANGE || number < -32768 || number > 32767) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value ") + integer_part + std::string(" is outside the range of values allowed in an i16")
		    );
	    return false;
	}
	result.parsed_type = type_part;
	result.i16_value = (short)number;
	return true;
    }
	break;
    case Type::TYPE_PRIMITIVE_i32:
    {
	long number = strtoll(source_cstring, &endptr, radix);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value contained extraneous characters: ") + integer_part
		    );
	    return false;
	}
	number = sign_positive ? number : -number;
	if (errno == ERANGE || number < -2147483648 || number > 2147483647) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value ") + integer_part + std::string(" is outside the range of values allowed in an i32")
		    );
	    return false;
	}
	result.parsed_type = type_part;
	result.i32_value = (int)number;
	return true;
    }
	break;
    case Type::TYPE_PRIMITIVE_i64:
    {
	long number = strtoll(source_cstring, &endptr, radix);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value contained extraneous characters: ") + integer_part
		    );
	    return false;
	}
	number = sign_positive ? number : -number;
	if (errno == ERANGE) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    literal_int_token.get_source_ref(),
		    "Invalid integer literal",
		    std::string("Literal value ") + integer_part + std::string(" is outside the range of values allowed in a u64")
		    );
	    return false;
	}
	result.parsed_type = type_part;
	result.i64_value = (long)number;
	return true;
    }
	break;

    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		literal_int_token.get_source_ref(),
		"Compiler Bug! Invalid integer literal",
		std::string("Unsupported primitive literal type ") + type_part->get_name()
		);
	return false;
    }
    /* Unreachable */
}

