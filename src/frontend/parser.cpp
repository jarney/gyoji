/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <gyoji-frontend.hpp>

#include <gyoji.l.hpp>
#include <gyoji.y.hpp>

using namespace Gyoji::context;
using namespace Gyoji::mir;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::ast;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::lowering;
using namespace Gyoji::frontend::namespaces;
using namespace Gyoji::frontend::yacc;

Gyoji::owned<ParseResult>
Parser::parse(
    Gyoji::context::CompilerContext & _compiler_context,
    Gyoji::misc::InputSource & _input_source
    )
{
    auto ns2_context = Gyoji::owned_new<Gyoji::frontend::namespaces::NS2Context>();
    Gyoji::owned<ParseResult> result = Gyoji::owned_new<ParseResult>(
	_compiler_context,
	std::move(ns2_context)
	);
    
    yyscan_t scanner;
    yylex_init(&scanner);
    
    LexContext lex_context(
	*result->ns2_context,
	_compiler_context,
	_input_source);
    yyset_extra(&lex_context, scanner);
    
    yacc::YaccParser parser { scanner, *result };
    parser.parse();
    yylex_destroy(scanner);
    
    return result;
}

Gyoji::owned<MIR>
Parser::parse_to_mir(
    Gyoji::context::CompilerContext & _compiler_context,
    Gyoji::misc::InputSource & _input_source,
    bool verbose
    )
{
    
    // We don't need to report an error at this point
    // because lack of a translation unit means
    // that our caller should not even have called us
    // and should report a syntax error at the
    // higher level.
    Gyoji::owned<ParseResult> parse_result = parse(_compiler_context, _input_source);
    Gyoji::owned<MIR> mir = Gyoji::owned_new<MIR>();
    
    if (!parse_result->has_translation_unit()) {
	// It's harmless to return an empty mir
	// to the next stages
	return mir;
    }

    Gyoji::mir::operation_static_init();

    if (verbose) {
	fprintf(stderr, "============================\n");
	fprintf(stderr, "Type and symbol table resolution pass\n");
	fprintf(stderr, "============================\n");
    }
    // First, resolve all of the type definitions.
    // Also at this stage, we resolve the function declarations.
    TypeLowering type_lowering(_compiler_context,
			       parse_result->get_translation_unit(),
			       *mir);
    type_lowering.lower();

    if (verbose) {
	fprintf(stderr, "============================\n");
	fprintf(stderr, "Function resolution pass\n");
	fprintf(stderr, "============================\n");
    }
    FunctionLowering function_lowering(_compiler_context,
				       *parse_result,
				       *mir,
				       type_lowering);
    function_lowering.lower();
    
    return mir;
}
