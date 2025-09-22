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
  
  define_type(std::make_unique<Type>("u8", Type::TYPE_PRIMITIVE, true, builtin_source_ref));

  define_type(std::make_unique<Type>("i16", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
  define_type(std::make_unique<Type>("i32", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
  define_type(std::make_unique<Type>("i64", Type::TYPE_PRIMITIVE, true, builtin_source_ref));

  define_type(std::make_unique<Type>("u16", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
  define_type(std::make_unique<Type>("u32", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
  define_type(std::make_unique<Type>("u64", Type::TYPE_PRIMITIVE, true, builtin_source_ref));

  define_type(std::make_unique<Type>("f32", Type::TYPE_PRIMITIVE, true, builtin_source_ref));
  define_type(std::make_unique<Type>("f64", Type::TYPE_PRIMITIVE, true, builtin_source_ref));

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

