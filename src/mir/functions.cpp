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

SimpleStatement::SimpleStatement()
{}

SimpleStatement::~SimpleStatement()
{}
/////////////////////////////////////
// BasicBlock
/////////////////////////////////////

BasicBlock::BasicBlock()
{}

BasicBlock::~BasicBlock()
{}
