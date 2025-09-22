#include <jlang-mir/types.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::context;
using namespace JLang::mir;

TypeMember::TypeMember(
                       std::string _member_name,
                       Type *_member_type,
                       const SourceReference & _source_ref
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

