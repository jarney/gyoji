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

TypeMember::TypeMember(
    std::string _member_name,
    size_t _index,
    const Type *_member_type,
    const SourceReference & _source_ref
    )
    : member_name(_member_name)
    , index(_index)
    , member_type(_member_type)
    , source_ref(&_source_ref)
{}
TypeMember::TypeMember(const TypeMember & other)
    : member_name(other.member_name)
    , index(other.index)
    , member_type(other.member_type)
    , source_ref(other.source_ref)
{}

TypeMember::~TypeMember()
{}

const std::string &
TypeMember::get_name() const
{ return member_name; }
const Type *
TypeMember::get_type() const
{ return member_type; }
const Gyoji::context::SourceReference &
TypeMember::get_source_ref() const
{ return *source_ref; }
size_t
TypeMember::get_index() const
{ return index; }
