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

const std::vector<JLang::owned<Function>> &
Functions::get_functions() const
{ return functions; }

/////////////////////////////////////
// Function
/////////////////////////////////////
Function::Function(
    std::string _name,
    std::string _return_type,
    std::vector<FunctionArgument> _arguments
    )
    : name(_name)
    , return_type(_return_type)
    , arguments(_arguments)
{}

Function::~Function()
{}

const std::string &
Function::get_return_type() const
{ return return_type; }

const std::string &
Function::get_name() const
{ return name; }

const std::vector<FunctionArgument> &

Function::get_arguments() const
{ return arguments; }

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
Function::dump() const
{
    fprintf(stderr, "Function %s returns %s\n", name.c_str(), return_type.c_str());
    for (const auto & arg : arguments) {
	fprintf(stderr, "    arg %s %s\n", arg.get_type().c_str(), arg.get_name().c_str());
    }
    for (size_t i = 0; i < blockid; i++) {
	fprintf(stderr, "BB%ld:\n", i);
	const BasicBlock & block = get_basic_block(i);
	block.dump();
    }
}
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
