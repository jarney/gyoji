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
#include <gyoji-mir/types.hpp>
#include <variant>
#include <stdio.h>

using namespace Gyoji::context;
using namespace Gyoji::mir;

TypeMethod::TypeMethod(
    std::string _method_name,
    const Gyoji::context::SourceReference & _source_ref,
    const Type *_class_type,
    const Type *_return_type,
    const std::vector<Argument> _arguments
    )
    : method_name(_method_name)
    , source_ref(_source_ref)
    , class_type(_class_type)
    , return_type(_return_type)
    , arguments(_arguments)
{}
TypeMethod::~TypeMethod()
{}

const std::string &
TypeMethod::get_name() const
{ return method_name; }

const Gyoji::context::SourceReference &
TypeMethod::get_source_ref() const
{ return source_ref; }

const Type *
TypeMethod::get_class_type() const
{ return class_type; }

const Type *
TypeMethod::get_return_type() const
{ return return_type; }

const std::vector<Argument> &
TypeMethod::get_arguments() const
{ return arguments; }
