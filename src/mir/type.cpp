#include <jlang-mir/types.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::context;
using namespace JLang::mir;

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

const SourceReference &
Type::get_declared_source_ref() const
{ return *declared_source_ref; }

const SourceReference &
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



