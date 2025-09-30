#pragma once

#include <jlang-mir/types.hpp>
#include <jlang-mir/operations.hpp>
#include <jlang-mir/functions.hpp>
#include <jlang-mir/symbols.hpp>

namespace JLang::mir {
    /**
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
    class MIR {
    public:
	MIR();
	~MIR();
	const Types & get_types() const;
	Types & get_types();

	const Symbols & get_symbols() const;
	Symbols & get_symbols();
	
	const Functions & get_functions() const;
	Functions & get_functions();
    private:
	Functions functions;
	Types types;
	Symbols symbols;
    };
};
