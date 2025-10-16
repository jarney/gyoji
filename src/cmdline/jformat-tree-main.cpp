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
#include "jformat-tree.hpp"

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::namespaces;
using namespace Gyoji::cmdline;

int main(int argc, char **argv)
{
    
    if (argc != 2) {
	fprintf(stderr, "Invalid number of arguments %d\n", argc);
	fprintf(stderr, "Usage: parser backend file\n");
	exit(1);
    }
    
    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", argv[1]);
	exit(1);
    }
    
    CompilerContext context(argv[1]);
    
    Gyoji::misc::InputSourceFile input_source(input);
    
    Gyoji::owned<ParseResult> parse_result = 
        Parser::parse(
	    context,
	    input_source
	    );
    close(input);
    if (parse_result->has_errors()) {
	parse_result->get_errors().print();
	return -1;
    }
    
    const TranslationUnit & translation_unit = parse_result->get_translation_unit();
    
    JFormatTree formatter;
    formatter.process(translation_unit.get_syntax_node());
    
    return 0;
}
