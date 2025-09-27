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

Type::TypeType
Type::get_type() const
{ return type; }
const std::string &
Type::get_name() const
{ return name; }

const std::vector<TypeMember> &
Type::get_members() const
{ return members; }

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
    else if (type == TYPE_FUNCTION_POINTER) {
	type_desc = std::string("fptr");
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
    else if (type == TYPE_FUNCTION_POINTER) {
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
    Type *_argument_type,
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

