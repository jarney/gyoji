#include <gyoji-mir/types.hpp>
#include <variant>
#include <stdio.h>
#include <gyoji-misc/jstring.hpp>

using namespace Gyoji::context;
using namespace Gyoji::mir;

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
    , declared_source_ref(&_source_ref)
    , defined_source_ref(&_source_ref)
    , pointer_or_ref(nullptr)
    , array_length(1)
    , return_type(nullptr)
    , argument_types()
    , members()
{}
Type::Type(std::string _name, const SourceReference & _source_ref, const Type & _other)
    : name(_name)
    , type(_other.type)
    , complete(_other.complete)
    , declared_source_ref(&_source_ref)
    , defined_source_ref(&_source_ref)
    , pointer_or_ref(_other.pointer_or_ref)
    , array_length(_other.array_length)
    , return_type(_other.return_type)
    , argument_types(_other.argument_types)
    , members(_other.members)
{
}

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

bool
Type::is_array() const
{ return (type == TYPE_ARRAY); }

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
    default:
	fprintf(stderr, "Compiler Bug!  This function should only be called for integer and floating-point types\n");
	exit(1);
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

const std::map<std::string, TypeMethod> &
Type::get_methods() const
{ return methods; }

const TypeMember *
Type::member_get(const std::string & member_name) const
{
    const auto it = members_by_name.find(member_name);
    if (it == members_by_name.end()) {
	return nullptr;
    }
    return it->second;
}

const Type *
Type::get_pointer_target() const
{ return pointer_or_ref; }

size_t
Type::get_array_length() const
{ return array_length; }

const Type*
Type::get_return_type() const
{ return return_type; }

const std::vector<Argument> &
Type::get_argument_types() const
{
    return argument_types;
}


void
Type::complete_pointer_definition(const Type *_type, const SourceReference & _source_ref)
{
    complete = true;
    pointer_or_ref = _type;
    defined_source_ref = &_source_ref;
}

void
Type::complete_array_definition(const Type *_type, size_t _array_length, const Gyoji::context::SourceReference & _source_ref)
{
    complete = true;
    pointer_or_ref = _type;
    array_length = _array_length;
    defined_source_ref = &_source_ref;
}

void
Type::complete_composite_definition(
    std::vector<TypeMember> _members,
    std::map<std::string, TypeMethod> _methods,
    const SourceReference & _source_ref)
{
    complete = true;
    members = _members;
    methods = _methods;
    for (const TypeMember & member : members) {
	members_by_name.insert(std::pair(member.get_name(), &member));
    }
    defined_source_ref = &_source_ref;
}

void
Type::complete_function_pointer_definition(
    const Type *_return_type,
    const std::vector<Argument> & _argument_types,
    const Gyoji::context::SourceReference & _source_ref
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
Type::dump(FILE *out) const
{
    std::string type_desc("unknown");
    
    if (!is_complete()) {
	type_desc = std::string("class (forward declaration)");
    }
    else if (is_primitive()) {
	type_desc = std::string("primitive");
    }
    else if (type == TYPE_COMPOSITE) {
	type_desc = std::string("class");
    }
    else if (is_pointer()) {
	type_desc = std::string("pointer");
	type_desc += std::string(" to ") + pointer_or_ref->get_name();
    }
    else if (is_reference()) {
	type_desc = std::string("reference");
	type_desc += std::string(" to ") + pointer_or_ref->get_name();
    }
    else if (is_enum()) {
	type_desc = std::string("enum");
    }
    else if (is_function_pointer()) {
	std::string desc;
	desc = desc + return_type->get_name();
	std::vector<std::string> arglist;
	for (const Argument & arg : argument_types) {
	    arglist.push_back(arg.get_type()->get_name());
	}
	desc = desc + std::string("(*)");
	desc = desc + std::string("(") + Gyoji::misc::join(arglist, ",") + std::string(")");
	type_desc = std::string("function-pointer ") + desc;
    }
    else if (is_array()) {
	type_desc = std::string("array ");
	type_desc += pointer_or_ref->get_name() + std::string("[") + std::to_string(array_length) + std::string("]");
    }
    else {
	fprintf(stderr, "Compiler Bug!  Unknown type of type when dumping types.\n");
	exit(1);
    }

    
    if (is_composite()) {
        fprintf(out, "    %s : %s {\n", name.c_str(), type_desc.c_str());
	for (const auto & m : members) {
	    fprintf(out, "        %s : %s\n", m.get_name().c_str(), m.get_type()->get_name().c_str());
	}
	for (const auto & m : methods) {
	    fprintf(out, "        %s : %s\n", m.first.c_str(), m.second.get_class_type()->get_name().c_str());
	    fprintf(out, "            ret %s\n", m.second.get_return_type()->get_name().c_str());
	    for (const auto & arg : m.second.get_arguments()) {
		fprintf(out, "            arg %s\n", arg.get_type()->get_name().c_str());
	    }
	}
	fprintf(out, "    }\n");
    }
    else {
        fprintf(out, "    %s : %s\n", name.c_str(), type_desc.c_str());
    }
}


/////////////////////////
// Argument
/////////////////////////
Argument::Argument(
    const Type *_argument_type,
    const Gyoji::context::SourceReference & _source_ref
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

const Gyoji::context::SourceReference &
Argument::get_source_ref() const
{ return *source_ref; }

