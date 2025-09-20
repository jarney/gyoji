#pragma once

#include <functional>
#include <list>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include <jlang-misc/pointers.hpp>
#include <jlang-misc/input-source.hpp>
#include <jlang-context/errors.hpp>
#include <jlang-context/token-stream.hpp>

#define _JLANG_INTERNAL
#include <jlang-frontend/forward.hpp>
#include <jlang-frontend/namespace.hpp>
#include <jlang-frontend/syntax-node.hpp>
#include <jlang-frontend/tree.hpp>
#include <jlang-frontend/parse-result.hpp>
#include <jlang-frontend/parser.hpp>
#undef _JLANG_INTERNAL



//! All aspects of the JLang language belong in this namespace. 
/*!
 * The JLang namespace contains the main components of the
 * compiler.
 */
namespace JLang {
    
};

/*!
 *  \addtogroup Frontend
 *  @{
 */
//! This is the front-end to the JLang parser.
/*!
 * This is the language front-end.
 * The purpose is to take an input
 * byte-stream and assemble an immutable
 * parse tree.
 */

namespace JLang::frontend {
};

/*! @} End of Doxygen Groups*/
