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
#include <gyoji-analysis.hpp>
#include <stdio.h>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::analysis;

AnalysisPass::AnalysisPass(CompilerContext & _compiler_context, std::string _name)
    : compiler_context(_compiler_context)
    , name(_name)
{}

AnalysisPass::~AnalysisPass()
{}

Gyoji::context::CompilerContext &
AnalysisPass::get_compiler_context() const
{ return compiler_context; }

const std::string &
AnalysisPass::get_name() const
{ return name; }
