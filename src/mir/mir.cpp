#include <jlang-mir.hpp>

using namespace JLang::mir;

MIR::MIR()
{}

MIR::~MIR()
{}

const Types &
MIR::get_types() const
{ return types; }

Types &
MIR::get_types()
{ return types; }

const Functions &
MIR::get_functions() const
{ return functions; }

Functions &
MIR::get_functions()
{ return functions; }
