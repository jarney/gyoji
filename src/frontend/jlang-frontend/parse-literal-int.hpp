#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend::integers {

    typedef struct {
	const JLang::mir::Type *parsed_type;
	
	unsigned char u8_value;
	unsigned short u16_value;
	unsigned int u32_value;
	unsigned long u64_value;
	
	char i8_value;
	short i16_value;
	int i32_value;
	long i64_value;
	
    } ParseLiteralIntResult;

    bool parse_literal_int(
	const JLang::context::CompilerContext & compiler_context,
	const JLang::mir::Types & types,
	const JLang::frontend::tree::Terminal & literal_int_token,
	ParseLiteralIntResult & result
	);

}
