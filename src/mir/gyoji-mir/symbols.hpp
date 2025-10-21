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
#pragma once

#include <gyoji-misc/pointers.hpp>
#include <gyoji-mir/types.hpp>

#include <string>
#include <map>
#include <vector>

namespace Gyoji::mir {

    /**
     * @brief A symbol defined globally inside a namespace.
     *
     * @details
     * This class holds a symbol which exists in the global
     * space of a translation unit.  For example, all global
     * variables and references to functions are defined as symbols
     * in the symbol table.  The symbol table consists of a
     * name that is unique in the global namespace and the associated
     * type as either a value type or function-pointer type (basically
     * anything except void).
     */
    class Symbol {
    public:
	typedef enum {
	    SYMBOL_STATIC_FUNCTION,
	    SYMBOL_MEMBER_METHOD,
	    SYMBOL_MEMBER_DESTRUCTOR
	} SymbolType;
	/**
	 * Creates a new symbol in the symbol table.  Names
	 * are assumed to be globally unique and care must
	 * be taken when adding symbols to ensure that
	 * they do not conflict since this would pose
	 * a problem for linkage of the generated binary.
	 */
	Symbol(std::string _name, SymbolType _type, const Type *_mir_type);

	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~Symbol();

	/**
	 * Returns the name of the symbol.
	 */
	std::string get_name() const;
	/**
	 * Returns a pointer to the immutable type
	 * of the symbol.
	 */
	const Type *get_mir_type() const;

	SymbolType get_type() const;
    private:
	std::string name;
	SymbolType type;
	const Type *mir_type;
    };

    /**
     * @brief Symbol table for the intermediate representation (MIR)
     *
     * @details
     *
     * This class represents the table of all globally defined
     * symbols such as global variables and function names.
     * The names must be unique and care should be taken to
     * always check whether a name already exists in the table
     * when defining new symbols.  It is the responsibility
     * of the front-end to construct globally unique names.
     *
     * This class is the owner of all symbols and they will
     * be destroyed when the symbol table goes out of scope.
     */
    class Symbols {
    public:
	/**
	 * Creates a new symbol table.
	 */
	Symbols();
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~Symbols();

	/**
	 * @brief Create a new global symbol.
	 *
	 * @details
	 * Defines a new symbol.  Any existing symbol will
	 * This will be a no-op if the symbol name already
	 * exists.  The caller is advised to always check
	 * that a symbol is defined before defining a new one.
	 */
	void define_symbol(
	    std::string name,
	    Symbol::SymbolType _sym_type,
	    const Type *symbol_type);

	/**
	 * @brief Look up a symbol by name.
	 *
	 * @details
	 * This will return the symbol given by name
	 * if that symbol exists.  If it does not exist,
	 * then nullptr will be returned instead.
	 */
	const Symbol * get_symbol(std::string name) const;

	/**
	 * This is used to dump the content of the global
	 * symbol table for debugging purposes.
	 */
	void dump(FILE *out) const;
    private:
	std::map<std::string, Gyoji::owned<Symbol>> symbols;
    };

};

