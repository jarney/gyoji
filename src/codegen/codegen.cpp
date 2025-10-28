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
using namespace Gyoji::codegen;

void Gyoji::codegen::generate_code(
    const Gyoji::context::CompilerContext & _compiler_context,
    const MIR & _mir,
    const CodeGeneratorLLVMOptions & _options
    )
{
    CodeGeneratorLLVM generator(_compiler_context, _mir, _options);
    generator.initialize();
    generator.generate();
    generator.output(_options.get_output_filename());
}

CodeGeneratorLLVMOptions::CodeGeneratorLLVMOptions()
{}

CodeGeneratorLLVMOptions::~CodeGeneratorLLVMOptions()
{}
	

bool
CodeGeneratorLLVMOptions::get_output_llvm_ir() const
{ return output_llvm_ir; }

void
CodeGeneratorLLVMOptions::set_output_llvm_ir(bool _output_llvm_ir)
{ output_llvm_ir = _output_llvm_ir; }

const std::string &
CodeGeneratorLLVMOptions::get_output_filename() const
{ return output_filename; }

void
CodeGeneratorLLVMOptions::set_output_filename(std::string _output_filename)
{ output_filename = _output_filename; }

void
CodeGeneratorLLVMOptions::set_optimization_level(int level)
{ optimization_level = level; }

int
CodeGeneratorLLVMOptions::get_optimization_level() const
{ return optimization_level; }

bool
CodeGeneratorLLVMOptions::get_verbose() const
{ return verbose; }

void
CodeGeneratorLLVMOptions::set_verbose(bool _verbose)
{ verbose = _verbose; }
