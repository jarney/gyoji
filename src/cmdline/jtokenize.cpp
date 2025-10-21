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
#include <gyoji-misc/input-source-file.hpp>
#include <gyoji-frontend.hpp>
#include <gyoji-context.hpp>
#include <gyoji.l.hpp>
#include <gyoji.y.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::yacc;

int main(int argc, char **argv)
{
    if (argc != 2) {
	fprintf(stderr, "Invalid number of arguments %d\n", argc);
	fprintf(stderr, "Usage: jformat-identity file\n");
	exit(1);
    }
    
    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", argv[1]);
	exit(1);
    }
    
    CompilerContext context(argv[1]);
    Gyoji::frontend::namespaces::NS2Context ns2_context;
    
    Gyoji::misc::InputSourceFile input_source(input);
    
    LexContext lex_context(
	ns2_context,
	context,
	input_source);
    
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_extra(&lex_context, scanner);
    
    while (true) {
	Gyoji::frontend::yacc::YaccParser::semantic_type lvalue;
	int rc = yylex (&lvalue, scanner);
	if (rc == 0) {
	    break;
	}
	const Gyoji::owned<Gyoji::frontend::tree::Terminal> & token = lvalue.as<Gyoji::owned<Gyoji::frontend::tree::Terminal>>();
	printf("%ld %ld : %d %s : %s\n",
	       token->get_source_ref().get_line(),
	       token->get_source_ref().get_column(),
	       token->get_type(),
	       token->get_name().c_str(),
	       token->get_fully_qualified_name().c_str()
	    );
    }
    
    return 0;
}
