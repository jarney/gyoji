#include <gyoji-mir.hpp>

using namespace Gyoji::mir;

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

Symbols &
MIR::get_symbols()
{ return symbols; }

const Symbols &
MIR::get_symbols() const
{ return symbols; }

void
MIR::dump(FILE *out) const
{
    fprintf(out, "Types:\n");
    types.dump(out);
    fprintf(out, "Symbol Table:\n");
    symbols.dump(out);
    fprintf(out, "Function Definitions\n");
    functions.dump(out);
}
