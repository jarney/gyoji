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

////////////////////////////////////////
// Types
////////////////////////////////////////

const static std::string zero_source_filename("builtin");
static SourceReference zero_source_ref(zero_source_filename, 0, 0, 0);

Types::Types()
{
    // XXX Definitely not the place to do this, but
    // it's a good enough place for now that I don't care
    // until we have a type system we can plug into this
    
    define_type(Gyoji::owned_new<Type>("i8", Type::TYPE_PRIMITIVE_i8, true, zero_source_ref));
    define_type(Gyoji::owned_new<Type>("i16", Type::TYPE_PRIMITIVE_i16, true, zero_source_ref));
    define_type(Gyoji::owned_new<Type>("i32", Type::TYPE_PRIMITIVE_i32, true, zero_source_ref));
    define_type(Gyoji::owned_new<Type>("i64", Type::TYPE_PRIMITIVE_i64, true, zero_source_ref));
    
    define_type(Gyoji::owned_new<Type>("u8", Type::TYPE_PRIMITIVE_u8, true, zero_source_ref));
    define_type(Gyoji::owned_new<Type>("u16", Type::TYPE_PRIMITIVE_u16, true, zero_source_ref));
    define_type(Gyoji::owned_new<Type>("u32", Type::TYPE_PRIMITIVE_u32, true, zero_source_ref));
    define_type(Gyoji::owned_new<Type>("u64", Type::TYPE_PRIMITIVE_u64, true, zero_source_ref));
    
    define_type(Gyoji::owned_new<Type>("f32", Type::TYPE_PRIMITIVE_f32, true, zero_source_ref));
    define_type(Gyoji::owned_new<Type>("f64", Type::TYPE_PRIMITIVE_f64, true, zero_source_ref));
    
    define_type(Gyoji::owned_new<Type>("bool", Type::TYPE_PRIMITIVE_bool, true, zero_source_ref));
    define_type(Gyoji::owned_new<Type>("void", Type::TYPE_PRIMITIVE_void, true, zero_source_ref));
}

Types::~Types()
{}

Type *
Types::get_type(std::string type) const
{
    const auto & it = type_map.find(type);
    if (it == type_map.end()) {
	return nullptr;
    }
    return it->second.get();
}
const Type *
Types::get_pointer_to(const Type *_type, const SourceReference & src_ref)
{
    std::string pointer_type_name = _type->get_name() + std::string("*");
    const Type* pointer_type = get_type(pointer_type_name);
    if (pointer_type != nullptr) {
	return pointer_type;
    }
    Gyoji::owned<Type> pointer_owned = Gyoji::owned_new<Type>(pointer_type_name, Type::TYPE_POINTER, false, src_ref);
    pointer_type = pointer_owned.get();
    pointer_owned->complete_pointer_definition(_type, src_ref);
    define_type(std::move(pointer_owned));
    return pointer_type;
}

const Type *
Types::get_reference_to(const Type *_type, const SourceReference & src_ref)
{
    std::string pointer_type_name = _type->get_name() + std::string("&");

    const Type* pointer_type = get_type(pointer_type_name);
    if (pointer_type != nullptr) {
	return pointer_type;
    }
    Gyoji::owned<Type> pointer_owned = Gyoji::owned_new<Type>(pointer_type_name, Type::TYPE_REFERENCE, false, src_ref);
    pointer_type = pointer_owned.get();
    pointer_owned->complete_pointer_definition(_type, src_ref);
    define_type(std::move(pointer_owned));
    return pointer_type;
}

const Type *
Types::get_array_of(
    const Type *_type,
    size_t _length,
    const SourceReference & _src_ref)
{
    std::string array_type_name = _type->get_name() + std::string("[") + std::to_string(_length) + std::string("]");

    const Type* array_type = get_type(array_type_name);
    if (array_type != nullptr) {
	return array_type;
    }
    Gyoji::owned<Type> array_owned = Gyoji::owned_new<Type>(array_type_name, Type::TYPE_ARRAY, false, _src_ref);
    array_type = array_owned.get();
    array_owned->complete_array_definition(_type, _length, _src_ref);
    define_type(std::move(array_owned));
    return array_type;
}

void
Types::define_type(Gyoji::owned<Type> type)
{
    std::string type_name = type->get_name();
    type_map.insert(std::pair<std::string, Gyoji::owned<Type>>(type_name, std::move(type)));
}

const std::map<std::string, Gyoji::owned<Type>> &
Types::get_types() const
{ return type_map; }


void
Types::dump(FILE *out) const
{
    for (const auto & t : type_map) {
	t.second->dump(out);
    }
}

