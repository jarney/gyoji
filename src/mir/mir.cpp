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
#include <gyoji-mir.hpp>

using namespace Gyoji::mir;

MIR::MIR()
{}

MIR::~MIR()
{}

const Types &
MIR::get_types() const
{ return types; }

Types &
MIR::get_types()
{ return types; }

const Functions &
MIR::get_functions() const
{ return functions; }

Functions &
MIR::get_functions()
{ return functions; }

Symbols &
MIR::get_symbols()
{ return symbols; }

const Symbols &
MIR::get_symbols() const
{ return symbols; }

void
MIR::dump(FILE *out) const
{
    fprintf(out, "Types:\n");
    types.dump(out);
    fprintf(out, "Symbol Table:\n");
    symbols.dump(out);
    fprintf(out, "Function Definitions\n");
    functions.dump(out);
}
