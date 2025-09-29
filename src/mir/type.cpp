#include <jlang-mir/types.hpp>
#include <variant>
#include <stdio.h>
#include <jlang-misc/jstring.hpp>

using namespace JLang::context;
using namespace JLang::mir;

////////////////////////////////////////
// Type
////////////////////////////////////////
Type::Type(
    std::string _name,
    TypeType _type,
    bool _complete,
    const SourceReference & _source_ref
    )
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

bool
Type::is_primitive() const
{
    return
	(type == TYPE_PRIMITIVE_u8) ||
	(type == TYPE_PRIMITIVE_u16) ||
	(type == TYPE_PRIMITIVE_u32) ||
	(type == TYPE_PRIMITIVE_u64) ||
	
	(type == TYPE_PRIMITIVE_i8) ||
	(type == TYPE_PRIMITIVE_i16) ||
	(type == TYPE_PRIMITIVE_i32) ||
	(type == TYPE_PRIMITIVE_i64) ||

	(type == TYPE_PRIMITIVE_f32) ||
	(type == TYPE_PRIMITIVE_f64) ||

	(type == TYPE_PRIMITIVE_bool) ||
	(type == TYPE_PRIMITIVE_void)
	;
}

bool
Type::is_pointer() const
{ return (type == TYPE_POINTER); }

bool
Type::is_reference() const
{ return (type == TYPE_REFERENCE); }

bool
Type::is_numeric() const
{
    return 
	(type == TYPE_PRIMITIVE_u8) ||
	(type == TYPE_PRIMITIVE_u16) ||
	(type == TYPE_PRIMITIVE_u32) ||
	(type == TYPE_PRIMITIVE_u64) ||
	
	(type == TYPE_PRIMITIVE_i8) ||
	(type == TYPE_PRIMITIVE_i16) ||
	(type == TYPE_PRIMITIVE_i32) ||
	(type == TYPE_PRIMITIVE_i64) ||

	(type == TYPE_PRIMITIVE_f32) ||
	(type == TYPE_PRIMITIVE_f64);
}

bool
Type::is_integer() const
{
    return 
	(type == TYPE_PRIMITIVE_u8) ||
	(type == TYPE_PRIMITIVE_u16) ||
	(type == TYPE_PRIMITIVE_u32) ||
	(type == TYPE_PRIMITIVE_u64) ||
	
	(type == TYPE_PRIMITIVE_i8) ||
	(type == TYPE_PRIMITIVE_i16) ||
	(type == TYPE_PRIMITIVE_i32) ||
	(type == TYPE_PRIMITIVE_i64);
}

bool
Type::is_signed() const
{
    return
	(type == TYPE_PRIMITIVE_i8) ||
	(type == TYPE_PRIMITIVE_i16) ||
	(type == TYPE_PRIMITIVE_i32) ||
	(type == TYPE_PRIMITIVE_i64);
}

bool
Type::is_unsigned() const
{
    return
	(type == TYPE_PRIMITIVE_u8) ||
	(type == TYPE_PRIMITIVE_u16) ||
	(type == TYPE_PRIMITIVE_u32) ||
	(type == TYPE_PRIMITIVE_u64);
}

bool
Type::is_float() const
{
    return
	(type == TYPE_PRIMITIVE_f32) ||
	(type == TYPE_PRIMITIVE_f64);
}

bool
Type::is_bool() const
{
    return 
	(type == TYPE_PRIMITIVE_bool);
}

bool
Type::is_void() const
{
    return
	(type == TYPE_PRIMITIVE_void);
}
bool
Type::is_enum() const
{ return (type == TYPE_ENUM); }

bool
Type::is_composite() const
{ return (type == TYPE_COMPOSITE); }

bool
Type::is_function_pointer() const
{ return (type == TYPE_FUNCTION_POINTER); }

size_t
Type::get_primitive_size() const
{
    switch (type) {
    case TYPE_PRIMITIVE_u8:
    case TYPE_PRIMITIVE_i8:
	return 1;
    case TYPE_PRIMITIVE_u16:
    case TYPE_PRIMITIVE_i16:
	return 2;
    case TYPE_PRIMITIVE_u32:
    case TYPE_PRIMITIVE_i32:
    case TYPE_PRIMITIVE_f32:
	return 4;
    case TYPE_PRIMITIVE_u64:
    case TYPE_PRIMITIVE_i64:
    case TYPE_PRIMITIVE_f64:
	return 8;
    }
    return 0;
    
}


Type::TypeType
Type::get_type() const
{ return type; }
const std::string &
Type::get_name() const
{ return name; }

const std::vector<TypeMember> &
Type::get_members() const
{ return members; }

const TypeMember *
Type::member_get(const std::string & member_name) const
{
    for (const auto & member : members) {
	if (member.get_name() == member_name) {
	    return &member;
	}
    }
    return nullptr;
}

const Type *
Type::get_pointer_target() const
{ return pointer_or_ref; }

const Type*
Type::get_return_type() const
{ return return_type; }

const std::vector<Argument> &
Type::get_argument_types() const
{
    return argument_types;
}


void
Type::complete_pointer_definition(const Type *_type, const SourceReference & source_ref)
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

void
Type::complete_function_pointer_definition(
    const Type *_return_type,
    const std::vector<Argument> & _argument_types,
    const JLang::context::SourceReference & _source_ref
    )
{
    complete = true;
    return_type = _return_type;
    argument_types = _argument_types;
    defined_source_ref = &_source_ref;
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
    
    if (is_primitive()) {
	type_desc = std::string("primitive");
    }
    else if (type == TYPE_COMPOSITE) {
	type_desc = std::string("composite");
    }
    else if (is_pointer()) {
	type_desc = std::string("pointer");
    }
    else if (is_reference()) {
	type_desc = std::string("reference");
    }
    else if (is_enum()) {
	type_desc = std::string("enum");
    }
    else if (is_function_pointer()) {
	type_desc = std::string("fptr");
    }
    
    if (!is_complete()) {
	fprintf(stderr, "(incomplete) ");
    }
    if (is_primitive()) {
	fprintf(stderr, "Type %s : %s\n",
		name.c_str(), type_desc.c_str());
    }
    else if (is_composite()) {
	fprintf(stderr, "Type %s : %s\n",
		name.c_str(), type_desc.c_str());
	
	fprintf(stderr, "{\n");
	for (const auto & m : members) {
	    fprintf(stderr, "    %s %s\n", m.get_type()->get_name().c_str(), m.get_name().c_str());
	}
	fprintf(stderr, "}\n");
    }
    else if (is_pointer()) {
	fprintf(stderr, "Type %s : %s to %s\n",
		name.c_str(), type_desc.c_str(), pointer_or_ref->get_name().c_str());
    }
    else if (is_reference()) {
	fprintf(stderr, "Type %s : %s to %s\n",
		name.c_str(), type_desc.c_str(), pointer_or_ref->get_name().c_str());
    }
    else if (is_function_pointer()) {
	std::string desc;
	desc = desc + return_type->get_name();
	std::vector<std::string> arglist;
	for (const Argument & arg : argument_types) {
	    arglist.push_back(arg.get_type()->get_name());
	}
	desc = desc + std::string("(*)");
	desc = desc + std::string("(") + JLang::misc::join(arglist, ",") + std::string(")");
	fprintf(stderr, "Type %s : pointer to a function %s : %s\n", type_desc.c_str(), name.c_str(), desc.c_str());
    }
}


/////////////////////////
// Argument
/////////////////////////
Argument::Argument(
    const Type *_argument_type,
    const JLang::context::SourceReference & _source_ref
    )
    : argument_type(_argument_type)
    , source_ref(&_source_ref)
{}
Argument::Argument(const Argument & _other)
    : argument_type(_other.argument_type)
    , source_ref(_other.source_ref)
{}
Argument::~Argument()
{}
const Type*
Argument::get_type() const
{ return argument_type; }

const JLang::context::SourceReference &
Argument::get_source_ref()
{ return *source_ref; }

