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

const FunctionPrototype *
Functions::get_prototype(std::string mangled_name) const
{
    const auto & it = prototypes.find(mangled_name);
    if (it == prototypes.end()) {
	return nullptr;
    }
    return it->second.get();
}

void
Functions::add_prototype(JLang::owned<FunctionPrototype> prototype)
{
    std::string mangled_name = prototype->get_name();
    prototypes.insert(std::pair(mangled_name, std::move(prototype)));
}

void
Functions::add_function(JLang::owned<Function> _function)
{
    functions.push_back(std::move(_function));
}

const Function * 
Functions::function_get(std::string _name) const
{
    for (const auto & fn : functions) {
	if (fn->get_prototype().get_name() == _name) {
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
LocalVariable::LocalVariable(std::string _name, std::string _type)
    : name(_name)
    , type(_type)
{}
LocalVariable::LocalVariable(const LocalVariable & _other)
    : name(_other.name)
    , type(_other.type)
{}
LocalVariable::~LocalVariable()
{}

/////////////////////////////////////
// Function
/////////////////////////////////////
Function::Function(
    const FunctionPrototype & _prototype,
    const JLang::context::SourceReference & _source_ref
    )
    : prototype(_prototype)
    , source_ref(_source_ref)
    , blockid(0)
{}

Function::~Function()
{}

const JLang::context::SourceReference &
Function::get_source_ref() const
{ return source_ref; }

const FunctionPrototype &
Function::get_prototype() const
{ return prototype; }

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
    fprintf(stderr, "Pushing block %ld\n", blockid);
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

void
Function::remove_local(std::string local_name)
{
    in_scope_locals.erase(local_name);
}

void
Function::dump() const
{
    fprintf(stderr, "Function %s returns %s\n", prototype.get_name().c_str(), prototype.get_return_type().c_str());
    for (const auto & arg : prototype.get_arguments()) {
	fprintf(stderr, "    arg %s %s\n", arg.get_type().c_str(), arg.get_name().c_str());
    }
    for (const size_t & i : blocks_in_order) {
	fprintf(stderr, "BB%ld:\n", i);
	const BasicBlock & block = get_basic_block(i);
	block.dump();
    }
}
/////////////////////////////////////
// FunctionPrototype
/////////////////////////////////////
FunctionPrototype::FunctionPrototype(
    std::string _name,
    std::string _return_type,
    std::vector<FunctionArgument> _arguments
    )
    : name(_name)
    , return_type(_return_type)
    , arguments(_arguments)
{}

FunctionPrototype::~FunctionPrototype()
{}

const std::string &
FunctionPrototype::get_return_type() const
{ return return_type; }

const std::string &
FunctionPrototype::get_name() const
{ return name; }

const std::vector<FunctionArgument> &
FunctionPrototype::get_arguments() const
{ return arguments; }

/////////////////////////////////////
// FunctionArgument
/////////////////////////////////////
FunctionArgument::FunctionArgument(
    std::string & _name,
    std::string & _type
    )
    : name(_name)
    , type(_type)
{}

FunctionArgument::~FunctionArgument()
{}

const std::string &
FunctionArgument::get_name() const
{ return name; }

const std::string &
FunctionArgument::get_type() const
{ return type; }

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
