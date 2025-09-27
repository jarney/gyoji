#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir/types.hpp>

#include <string>
#include <map>
#include <vector>

namespace JLang::mir {

    //! A symbol defined globally inside a namespace.
    class Symbol {
    public:
	Symbol(std::string _name, const Type *_type);
	~Symbol();
	std::string get_name() const;
	const Type *get_type() const;
    private:
	std::string name;
	const Type *type;
    };

    class Symbols {
    public:
	Symbols();
	~Symbols();

	void define_symbol(std::string name, const Type *symbol_type);
	const Symbol * get_symbol(std::string name) const;
    private:
	std::map<std::string, JLang::owned<Symbol>> symbols;
    };

};

