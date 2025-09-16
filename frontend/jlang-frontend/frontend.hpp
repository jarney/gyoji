#pragma once

#include <functional>
#include <list>
#include <map>
#include <string>
#include <memory>
#include <variant>

#define _JLANG_INTERNAL
#include <jlang-frontend/forward.hpp>
#include <jlang-frontend/namespace.hpp>
#include <jlang-frontend/syntax-node.hpp>
#include <jlang-frontend/tree.hpp>
#include <jlang-frontend/yacc-context.hpp>
#include <jlang-frontend/lex-context.hpp>
#include <jlang-frontend/parser.hpp>
#undef _JLANG_INTERNAL



//! This is the top-level namespace for the project.
/*!
 * All features belong in this namespace.
 */
namespace JLang {
    
};

/*!
 *  \addtogroup Frontend
 *  @{
 */
//! Headline News for the front-end.
/*!
 * This is the language front-end.
 * The purpose is to take an input
 * byte-stream and assemble an immutable
 * parse tree.
 */

namespace JLang::frontend {
};

/*! @} End of Doxygen Groups*/
