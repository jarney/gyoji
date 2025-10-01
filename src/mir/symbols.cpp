#include <jlang-mir/symbols.hpp>

using namespace JLang::mir;

Symbol::Symbol(std::string _name, const Type *_type)
    : name(_name)
    , type(_type)
{}

Symbol::~Symbol()
{}

std::string
Symbol::get_name() const
{ return name; }

const Type *
Symbol::get_type() const
{ return type; }



Symbols::Symbols()
{}

Symbols::~Symbols()
{}

void
Symbols::define_symbol(std::string name, const Type *symbol_type)
{
    symbols.insert(std::pair(name, std::make_unique<Symbol>(name, symbol_type)));
}

void
Symbols::dump(FILE *out) const
{
    for (const auto & symbol : symbols) {
	const JLang::owned<Symbol> & sym = symbol.second;
        fprintf(out, "    %s : %s\n",
		sym->get_name().c_str(),
		sym->get_type()->get_name().c_str());
    }
}


const Symbol *
Symbols::get_symbol(std::string name) const
{
    const auto & it = symbols.find(name);
    if (it == symbols.end()) {
	return nullptr;
    }
    return it->second.get();
}
