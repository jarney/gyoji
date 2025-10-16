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
#define _GYOJI_INTERNAL
#include <gyoji-frontend/lex-context.hpp>
#undef _GYOJI_INTERNAL
#include <gyoji.l.hpp>
#include <gyoji.y.hpp>

using namespace Gyoji::frontend::yacc;

LexContext::LexContext(
    Gyoji::frontend::namespaces::NS2Context &_ns2_context,
    Gyoji::context::CompilerContext & _compiler_context,
    Gyoji::misc::InputSource &_input_source)
    : ns2_context(_ns2_context)
    , input_source(_input_source)
    , compiler_context(_compiler_context)
    , line(1)
    , column(0)
{}

LexContext::~LexContext()
{}
