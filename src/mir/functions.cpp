#include <jlang-mir/functions.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::mir;

/////////////////////////////////////
// Functions
/////////////////////////////////////
Functions::Functions()
{}

Functions::~Functions()
{}

void
Functions::add_function(JLang::owned<Function> _function)
{
    functions.push_back(std::move(_function));
}

const Function * 
Functions::function_get(std::string _name) const
{
    for (const auto & fn : functions) {
	if (fn->get_name() == _name) {
	    return fn.get();
	}
    }
    return nullptr;
}

const std::vector<JLang::owned<Function>> &
Functions::get_functions() const
{ return functions; }

/////////////////////////////////////
// LocalVariable
/////////////////////////////////////
LocalVariable::LocalVariable(std::string _name, std::string _type, const JLang::context::SourceReference & _src_ref)
    : name(_name)
    , type(_type)
    , src_ref(_src_ref)
     
{}
LocalVariable::LocalVariable(const LocalVariable & _other)
    : name(_other.name)
    , type(_other.type)
    , src_ref(_other.src_ref)
{}
LocalVariable::~LocalVariable()
{}
/////////////////////////////////////
// TmpValue
/////////////////////////////////////
TmpValue::TmpValue(const std::string & _type)
    : type(_type)
{}
TmpValue::TmpValue(const TmpValue & _other)
    : type(_other.type)
{}

TmpValue::~TmpValue()
{}
const std::string &
TmpValue::get_type() const
{ return type; }

/////////////////////////////////////
// Function
/////////////////////////////////////
Function::Function(
    std::string _name,
    const Type *_return_type,
    const std::vector<FunctionArgument> & _arguments,
    const JLang::context::SourceReference & _source_ref
    )
    : name(_name)
    , return_type(_return_type)
    , arguments(_arguments)
    , source_ref(_source_ref)
    , blockid(0)
{}

Function::~Function()
{}

const std::string &
Function::get_name() const
{ return name; }

const Type *
Function::get_return_type() const
{ return return_type; }

const std::vector<FunctionArgument> &
Function::get_arguments() const
{ return arguments; }

const JLang::context::SourceReference &
Function::get_source_ref() const
{ return source_ref; }

const BasicBlock &
Function::get_basic_block(size_t blockid) const
{
    const auto & it = blocks.find(blockid);
    return *(it->second);
}

BasicBlock &
Function::get_basic_block(size_t blockid)
{
    const auto & it = blocks.find(blockid);
    return *(it->second);
}
size_t
Function::add_block()
{
    blocks[blockid] = std::make_unique<BasicBlock>();
    size_t blockid_created = blockid;
    blockid++;
    return blockid_created;
}
void
Function::push_block(size_t blockid)
{
    blocks_in_order.push_back(blockid);
}

const LocalVariable *
Function::get_local(std::string local_name)
{
    const auto & it = in_scope_locals.find(local_name);
    if (it != in_scope_locals.end()) {
	return &it->second;
    }
    return nullptr;
}

bool
Function::add_local(const LocalVariable & local)
{
    if (get_local(local.name) != nullptr) {
	return false;
    }
    in_scope_locals.insert(std::pair(local.name, local));
    return true;
}

const TmpValue *
Function::tmpvar_get(size_t tmpvar_id)
{ return &tmpvars.at(tmpvar_id); }

size_t
Function::tmpvar_define(std::string type_name)
{
    tmpvars.push_back(TmpValue(type_name));
    return tmpvars.size()-1;
}

void
Function::remove_local(std::string local_name)
{
    in_scope_locals.erase(local_name);
}

void
Function::dump() const
{
    fprintf(stderr, "Function %s %s\n", get_name().c_str(), return_type->get_name().c_str());
    for (const auto & arg : arguments) {
	fprintf(stderr, "    arg %s %s\n", arg.get_type()->get_name().c_str(), arg.get_name().c_str());
    }
    for (const size_t & i : blocks_in_order) {
	fprintf(stderr, "BB%ld:\n", i);
	const BasicBlock & block = get_basic_block(i);
	block.dump();
    }
}
/////////////////////////////////////
// SimpleStatement
/////////////////////////////////////

SimpleStatement::SimpleStatement(std::string _statement_desc)
    : statement_desc(_statement_desc)
{}

SimpleStatement::~SimpleStatement()
{}
const std::string & 
SimpleStatement::get_statement() const
{
    return statement_desc;
}
/////////////////////////////////////
// BasicBlock
/////////////////////////////////////

BasicBlock::BasicBlock()
{}

BasicBlock::~BasicBlock()
{}

void
BasicBlock::add_statement(std::string statement)
{
    JLang::owned<SimpleStatement> simple_statement = std::make_unique<SimpleStatement>(statement);
    statements.push_back(std::move(simple_statement));
}

void
BasicBlock::dump() const
{
    for (const auto & statement : statements) {
	fprintf(stderr, "    %s\n", statement->get_statement().c_str());
    }
}

/////////////////////////////////////
// FunctionArgument
/////////////////////////////////////
FunctionArgument::FunctionArgument(
    std::string & _name,
    const Type * _type
    )
    : name(_name)
    , type(_type)
{}

FunctionArgument::FunctionArgument(const FunctionArgument & _other)
    : name(_other.name)
    , type(_other.type)
{}

FunctionArgument::~FunctionArgument()
{}

const std::string &
FunctionArgument::get_name() const
{ return name; }

const Type*
FunctionArgument::get_type() const
{ return type; }
