#include <jlang-mir/types.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::context;
using namespace JLang::mir;

////////////////////////////////////////
// Types
////////////////////////////////////////

static JLang::context::SourceReference builtin_source_ref("builtin", 0, 0);

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
Type::Type(std::string _name, TypeType _type, bool _complete, const SourceReference & _source_ref)
  : name(_name)
  , type(_type)
  , complete(_complete)
  , defined_source_ref(&_source_ref)
  , declared_source_ref(&_source_ref)
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

const std::vector<TypeMember> &
Type::get_members() const
{ return members; }

void
Type::complete_pointer_definition(Type *_type, const SourceReference & source_ref)
{
  complete = true;
  pointer_or_ref = _type;
  defined_source_ref = &source_ref;
}

void
Type::complete_composite_definition(std::vector<TypeMember> _members, const SourceReference & source_ref)
{
  complete = true;
  members = _members;
  defined_source_ref = &source_ref;
}

const JLang::context::SourceReference &
Type::get_declared_source_ref() const
{ return *declared_source_ref; }

const JLang::context::SourceReference &
Type::get_defined_source_ref() const
{ return *defined_source_ref; }

void
Type::dump() const
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
      fprintf(stderr, "    %s %s\n", m.get_type()->get_name().c_str(), m.get_name().c_str());
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



TypeMember::TypeMember(
                       std::string _member_name,
                       Type *_member_type,
                       const JLang::context::SourceReference & _source_ref
                       )
  : member_name(_member_name)
  , member_type(_member_type)
  , source_ref(&_source_ref)
{}
TypeMember::TypeMember(const TypeMember & other)
  : member_name(other.member_name)
  , member_type(other.member_type)
  , source_ref(other.source_ref)
{}

TypeMember &
TypeMember::operator=(const TypeMember & other)
{
  member_name = other.member_name;
  member_type = other.member_type;
  source_ref = other.source_ref;
  return *this;
}

TypeMember::~TypeMember()
{}

const std::string &
TypeMember::get_name() const
{ return member_name; }
const Type *
TypeMember::get_type() const
{ return member_type; }
const JLang::context::SourceReference &
TypeMember::get_source_ref() const
{ return *source_ref; }

