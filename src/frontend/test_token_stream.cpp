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
#include <gyoji-misc/input-source-file.hpp>
#include <gyoji-misc/test.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::namespaces;

/*
 * The purpose of this file is to
 * perform a literal output of the token
 * stream exactly as it was read so that
 * we can verify that all original input
 * is available later when we need it
 * for printing error messages or
 * processing tokens.
 */

static
Gyoji::owned<ParseResult>
parse(std::string & filename, CompilerContext & context)
{
    int input = open(filename.c_str(), O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", filename.c_str());
	return nullptr;
    }
    
    Gyoji::misc::InputSourceFile input_source(input);
    Gyoji::owned<ParseResult> parse_result = 
	Parser::parse(
	    context,
	    input_source
	    );
    close(input);
    if (parse_result->has_errors()) {
	parse_result->get_errors().print();
    }
    
    return parse_result;
}


int main(int argc, char **argv)
{
    if (argc != 2) {
	fprintf(stderr, "First argument must be the path of the source directory\n");
	return -1;
    }
    
    std::string path(argv[1]);
    
    CompilerContext context(path);
    
    Gyoji::owned<ParseResult> result = parse(path, context);
    if (!result) {
	return 1;
    }
    
    for (const Gyoji::owned<Token> & token : result->get_token_stream().get_tokens()) {
	printf("%s", token->get_value().c_str());
    }
    
    return 0;
}
