#include <gyoji-mir/types.hpp>
#include <variant>
#include <stdio.h>

using namespace Gyoji::context;
using namespace Gyoji::mir;

TypeMethod::TypeMethod(
    std::string _method_name,
    const Gyoji::context::SourceReference & _source_ref,
    const Type *_class_type,
    const Type *_return_type,
    const std::vector<Argument> _arguments
    )
    : method_name(_method_name)
    , source_ref(_source_ref)
    , class_type(_class_type)
    , return_type(_return_type)
    , arguments(_arguments)
{}
TypeMethod::~TypeMethod()
{}

const std::string &
TypeMethod::get_name() const
{ return method_name; }

const Gyoji::context::SourceReference &
TypeMethod::get_source_ref() const
{ return source_ref; }

const Type *
TypeMethod::get_class_type() const
{ return class_type; }

const Type *
TypeMethod::get_return_type() const
{ return return_type; }

const std::vector<Argument> &
TypeMethod::get_arguments() const
{ return arguments; }
