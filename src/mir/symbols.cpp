/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <gyoji-mir/symbols.hpp>

using namespace Gyoji::mir;

Symbol::Symbol(
    std::string _name,
    SymbolType _type,
    const Type *_mir_type)
    : name(_name)
    , type(_type)
    , mir_type(_mir_type)
{}

Symbol::~Symbol()
{}

std::string
Symbol::get_name() const
{ return name; }

const Type *
Symbol::get_mir_type() const
{ return mir_type; }

Symbol::SymbolType
Symbol::get_type() const
{ return type; }


Symbols::Symbols()
{}

Symbols::~Symbols()
{}

void
Symbols::define_symbol(
    std::string name,
    Symbol::SymbolType type,
    const Type *mir_type)
{
    const auto & it = symbols.find(name);
    if (it != symbols.end()) {
	return;
    }
    symbols.insert(std::pair(name, std::make_unique<Symbol>(name, type, mir_type)));
}

void
Symbols::dump(FILE *out) const
{
    for (const auto & symbol : symbols) {
	const Gyoji::owned<Symbol> & sym = symbol.second;
	std::string type;
	switch (sym->get_type()) {
	case Symbol::SYMBOL_STATIC_FUNCTION:
	    type = std::string("static function");
	    break;
	case Symbol::SYMBOL_MEMBER_DESTRUCTOR:
	    type = std::string("member destructor");
	    break;
	case Symbol::SYMBOL_MEMBER_METHOD:
	    type = std::string("member method");
	    break;
	}
        fprintf(out, "    %s : %s %s\n",
		sym->get_name().c_str(),
		type.c_str(),
		sym->get_mir_type()->get_name().c_str());
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
