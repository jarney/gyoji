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
#include <gyoji-context.hpp>

using namespace Gyoji::context;

CompilerContext::CompilerContext(std::string _filename)
{
    token_stream = Gyoji::owned_new<TokenStream>();
    errors = Gyoji::owned_new<Errors>(*token_stream);
    filenames.push_back(_filename);
}
CompilerContext::~CompilerContext()
{}

bool
CompilerContext::has_errors() const
{ return errors->has_errors(); }

Errors &
CompilerContext::get_errors() const
{ return *errors; }

TokenStream &
CompilerContext::get_token_stream() const
{ return *token_stream; }

const std::string &
CompilerContext::get_filename() const
{ return filenames.back(); }

void
CompilerContext::add_filename(const std::string & _filename)
{
    filenames.push_back(_filename);
}
