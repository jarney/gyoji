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
#include <gyoji-frontend/type-lowering.hpp>
#include <gyoji-mir.hpp>
#include <gyoji-misc/test.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::namespaces;
using namespace Gyoji::mir;


static
Gyoji::owned<MIR>
parse_to_mir(std::string & path, CompilerContext & context, std::string base_filename);

int main(int argc, char **argv)
{
    if (argc != 2) {
	fprintf(stderr, "First argument must be the path of the source directory\n");
	return -1;
    }
    printf("Testing types\n");
    
    std::string path(argv[1]);

    const char *filename = "tests/type-resolution.j";
    CompilerContext context(filename);
    
    auto mir = parse_to_mir(path, context, filename);
    
    mir->get_types().dump(stderr);
    
    printf("    PASSED\n");
}

static
Gyoji::owned<MIR>
parse_to_mir(std::string & path, CompilerContext & context, std::string base_filename)
{
    std::string filename = path + std::string("/") + base_filename;
    
    int input = open(filename.c_str(), O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", base_filename.c_str());
	return nullptr;
    }
    
    Gyoji::misc::InputSourceFile input_source(input);
    auto mir =
	Parser::parse_to_mir(
	    context,
	    input_source
	    );
    close(input);
    return mir;
}
