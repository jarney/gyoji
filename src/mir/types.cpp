#include <jlang-mir/types.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::mir;

////////////////////////////////////////
// Types
////////////////////////////////////////

Types::Types()
{
  // XXX Definitely not the place to do this, but
  // it's a good enough place for now that I don't care
  // until we have a type system we can plug into this
  define_type(std::make_unique<Type>("u8", Type::TYPE_PRIMITIVE, true));

  define_type(std::make_unique<Type>("i16", Type::TYPE_PRIMITIVE, true));
  define_type(std::make_unique<Type>("i32", Type::TYPE_PRIMITIVE, true));
  define_type(std::make_unique<Type>("i64", Type::TYPE_PRIMITIVE, true));

  define_type(std::make_unique<Type>("u16", Type::TYPE_PRIMITIVE, true));
  define_type(std::make_unique<Type>("u32", Type::TYPE_PRIMITIVE, true));
  define_type(std::make_unique<Type>("u64", Type::TYPE_PRIMITIVE, true));

  define_type(std::make_unique<Type>("f32", Type::TYPE_PRIMITIVE, true));
  define_type(std::make_unique<Type>("f64", Type::TYPE_PRIMITIVE, true));

  define_type(std::make_unique<Type>("void", Type::TYPE_PRIMITIVE, true));
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



void
Types::dump()
{
  for (const auto & t : type_map) {
    t.second->dump();
  }
}

////////////////////////////////////////
// Type
////////////////////////////////////////
Type::Type(std::string _name, TypeType _type, bool _complete)
  : name(_name)
  , type(_type)
  , complete(_complete)
{}

Type::~Type()
{}

bool
Type::is_complete() const
{ return complete; }

Type::TypeType
Type::get_type() const
{ return type; }
const std::string &
Type::get_name() const
{ return name; }

const std::vector<std::pair<std::string, Type*>> &
Type::get_members() const
{ return members; }

void
Type::complete_pointer_definition(Type *_type)
{
  complete = true;
  pointer_or_ref = _type;
}

void
Type::complete_composite_definition(std::vector<std::pair<std::string, Type*>> _members)
{
  complete = true;
  members = _members;
}

void
Type::dump()
{
  std::string type_desc("unknown");
  
  if (type == TYPE_PRIMITIVE) {
    type_desc = std::string("primitive");
  }
  else if (type == TYPE_COMPOSITE) {
    type_desc = std::string("composite");
  }
  else if (type == TYPE_POINTER) {
    type_desc = std::string("pointer");
  }
  else if (type == TYPE_REFERENCE) {
    type_desc = std::string("reference");
  }
  else if (type == TYPE_ENUM) {
    type_desc = std::string("enum");
  }
  if (!is_complete()) {
    fprintf(stderr, "(incomplete) ");
  }
  if (type == TYPE_PRIMITIVE) {
    fprintf(stderr, "Type %s : %s\n",
            name.c_str(), type_desc.c_str());
  }
  else if (type == TYPE_COMPOSITE) {
    fprintf(stderr, "Type %s : %s\n",
            name.c_str(), type_desc.c_str());

    fprintf(stderr, "{\n");
    for (const auto & m : members) {
      fprintf(stderr, "    %s %s\n", m.second->get_name().c_str(), m.first.c_str());
    }
    fprintf(stderr, "}\n");
  }
  else if (type == TYPE_POINTER) {
    fprintf(stderr, "Type %s : %s to %s\n",
            name.c_str(), type_desc.c_str(), pointer_or_ref->get_name().c_str());
  }
  else if (type == TYPE_REFERENCE) {
    fprintf(stderr, "Type %s : %s to %s\n",
            name.c_str(), type_desc.c_str(), pointer_or_ref->get_name().c_str());
  }
}
