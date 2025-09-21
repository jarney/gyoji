#include <jlang-mir.hpp>

using namespace JLang::mir;

MIR::MIR()
{}

MIR::~MIR()
{}

Types &
MIR::get_types()
{ return types; }

Functions &
MIR::get_functions()
{ return functions; }
