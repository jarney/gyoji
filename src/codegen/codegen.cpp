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
#include <gyoji-codegen.hpp>
#include <gyoji-context.hpp>
#include <stdio.h>

using namespace Gyoji::mir;

void Gyoji::codegen::generate_code(
    const Gyoji::context::CompilerContext & _compiler_context,
    const MIR & _mir,
    const std::string & _filename
    )
{
    CodeGeneratorLLVM generator(_compiler_context, _mir);
    generator.initialize();  
    generator.generate();
    generator.output(_filename);
}
