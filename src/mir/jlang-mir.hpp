#pragma once

#include <jlang-mir/types.hpp>
#include <jlang-mir/operations.hpp>
#include <jlang-mir/functions.hpp>
#include <jlang-mir/symbols.hpp>

/**
 * @brief Middle intermediate representation (MIR) of
 *        a translation unit.
 *
 * @details
 * The MIR is an intermediate representation
 * of a source program.  It contains no syntax
 * information and just enough context from the
 * source-code that any errors can be traced back
 * to the source-code for quality error reporting.
 *
 * The purpose of the MIR is to provide a representation
 * that is at a low enough level that a code-generator
 * such as LLVM can be used to emit assembly,
 * while remaining high-level enough that we can
 * perform analysis and reasoning about the semantics
 * such as implementing a borrow checker.
 *
 * The MIR consists of two primary components.  The
 * first are the Types which represent logical layouts
 * of data.  The second are Functions which represent
 * operations that manipulate data (in types).
 *
 * The purpose of the MIR is to provide a consistent
 * view of the intermediate representation of each
 * of these things together as a unit because
 * many operations like code-generation and analysis
 * rely on them being paired and consistent.
 */
namespace JLang::mir {

    /**
     * @brief The middle-tier intermediate representation (MIR)
     *        of a translation unit.
     *
     * @details
     * The MIR class represents the semantic information
     * about everything expressed in a single translation-unit.
     * The main actors are Types, Symbols, anf Functions.
     * The Types represent data stored in a variable, or identifier.
     * The Symbols represent global variables and functions
     * that will ultimately wind up in the symbol table
     * of the generated binary file.  The Functions
     * represent the callable entities and contain the
     * blocks and operations it will perform including
     * primitive operations, flow-control instructions,
     * variable declarations, and things that happen
     * inside functions.
     */
    class MIR {
    public:
	MIR();
	~MIR();

	/**
	 * @brief Immutable type container.
	 *
	 * @details
	 * This method returns an immutable
	 * reference to the container of types.
	 * It is used after the front-end has
	 * finished creating the MIR and is used
	 * by analysis passes and code-generation
	 * passes which will not modify the MIR
	 * during the course of operations.
	 */
	const Types & get_types() const;
	/**
	 * @brief Mutable type container
	 *
	 * @details
	 * This method returns a mutable
	 * reference to the container of types.
	 * This is intended for use in constructing the
	 * MIR and should not be used after the front-end
	 * stage has finished parsing.
	 */
	Types & get_types();

	/**
	 * @brief Immutable symbol table.
	 *
	 * @details
	 * This method returns an immutable
	 * reference to the symbol table.
	 * It is used after the front-end has
	 * finished creating the MIR and is used
	 * by analysis passes and code-generation
	 * passes which will not modify the MIR
	 * during the course of operations.
	 */
	const Symbols & get_symbols() const;

	/**
	 * @brief
	 *
	 * @details
	 * This method returns a mutable reference
	 * to the symbol table.
	 *
	 * This is intended for use in constructing the
	 * MIR and should not be used after the front-end
	 * stage has finished parsing.
	 */
	Symbols & get_symbols();

	/**
	 * @brief Immutable function table.
	 *
	 * @details
	 * This method returns an immutable
	 * reference to the functions themselves.
	 *
	 * It is used after the front-end has
	 * finished creating the MIR and is used
	 * by analysis passes and code-generation
	 * passes which will not modify the MIR
	 * during the course of operations.
	 */
	const Functions & get_functions() const;
	/**
	 * @brief Mutable function table.
	 *
	 * @details
	 * This method returns a mutable reference
	 * to the functions themselves.
	 *
	 * This is intended for use in constructing the
	 * MIR and should not be used after the front-end
	 * stage has finished parsing.
	 */
	Functions & get_functions();

	/**
	 * @brief Dump MIR for debugging.
	 *
	 * @details
	 * This method is a debugging helper to assist
	 * in vizualizing the MIR so we can examine it
	 * as we work on other layers like the analysis
	 * and code-generation phases.
	 *
	 * The following gives a minimal example of what is contained
	 * in the MIR dump.  This gives a sense of the information
	 * available to the analysis and code-generation stages.
	 *
	 * @code{.unparsed}
	 * Types:
	 *     u32 : primitive
	 *     u8 : primitive
	 *     void : primitive
	 * Symbol Table:
	 *     preamble_function_call : u32(*)()
	 *     write : u32(*)(u32,u8*,u32)
	 * Function Definitions
	 *     preamble_function_call
	 *         return-value : u32
	 *     temporary variables
	 *       _0 : u32(*)(u32,u8*,u32)
	 *       _1 : u8
	 *       _2 : u8*
	 *       _3 : u32
	 *       _4 : u32
	 *     {
	 *       BB0:
	 *           _0 = symbol ( write )
	 *           _1 = literal-int ( 0 )
	 *           _2 = literal-string ( "abcXXdef" )
	 *           _3 = literal-int ( 5 )
	 *           _4 = function-call ( _0 _1 _2 _3 )
	 *     }
	 * @endcode
	 */
	void dump(FILE *out) const;
    private:
	Functions functions;
	Types types;
	Symbols symbols;
    };
};
