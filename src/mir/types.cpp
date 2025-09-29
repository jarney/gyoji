#include <jlang-mir/types.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::context;
using namespace JLang::mir;

////////////////////////////////////////
// Types
////////////////////////////////////////

static SourceReference builtin_source_ref("builtin", 0, 0);

Types::Types()
{
    // XXX Definitely not the place to do this, but
    // it's a good enough place for now that I don't care
    // until we have a type system we can plug into this
    
    define_type(std::make_unique<Type>("i8", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    define_type(std::make_unique<Type>("i16", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    define_type(std::make_unique<Type>("i32", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    define_type(std::make_unique<Type>("i64", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    
    define_type(std::make_unique<Type>("u8", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    define_type(std::make_unique<Type>("u16", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    define_type(std::make_unique<Type>("u32", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    define_type(std::make_unique<Type>("u64", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    
    define_type(std::make_unique<Type>("f32", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    define_type(std::make_unique<Type>("f64", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    
    define_type(std::make_unique<Type>("bool", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
    define_type(std::make_unique<Type>("void", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
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
    Type* pointer_type = get_type(pointer_type_name);
    if (pointer_type != nullptr) {
	return pointer_type;
    }
    JLang::owned<Type> pointer_owned = std::make_unique<Type>(pointer_type_name, Type::TYPE_POINTER, false, src_ref);
    pointer_type = pointer_owned.get();
    pointer_owned->complete_pointer_definition(_type, src_ref);
    define_type(std::move(pointer_owned));
    return pointer_type;
}

const Type *
Types::get_reference_to(const Type *_type, const SourceReference & src_ref)
{
    std::string pointer_type_name = _type->get_name() + std::string("&");

    Type* pointer_type = get_type(pointer_type_name);
    if (pointer_type != nullptr) {
	return pointer_type;
    }
    JLang::owned<Type> pointer_owned = std::make_unique<Type>(pointer_type_name, Type::TYPE_REFERENCE, false, src_ref);
    pointer_type = pointer_owned.get();
    pointer_owned->complete_pointer_definition(_type, src_ref);
    define_type(std::move(pointer_owned));
    return pointer_type;
}

void
Types::define_type(JLang::owned<Type> type)
{
    std::string type_name = type->get_name();
    type_map.insert(std::pair<std::string, JLang::owned<Type>>(type_name, std::move(type)));
}

const std::map<std::string, JLang::owned<Type>> &
Types::get_types() const
{ return type_map; }


void
Types::dump()
{
    for (const auto & t : type_map) {
	t.second->dump();
    }
}

