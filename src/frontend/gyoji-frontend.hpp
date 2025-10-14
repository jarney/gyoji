#pragma once

#include <functional>
#include <list>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include <gyoji-misc/pointers.hpp>
#include <gyoji-misc/input-source.hpp>
#include <gyoji-context.hpp>
#include <gyoji-mir.hpp>

#define _GYOJI_INTERNAL
#include <gyoji-frontend/forward.hpp>
#include <gyoji-frontend/tokens.hpp>
#include <gyoji-frontend/parse-literal-int.hpp>
#include <gyoji-frontend/ns2.hpp>
#include <gyoji-frontend/syntax-node.hpp>
#include <gyoji-frontend/tree.hpp>
#include <gyoji-frontend/parse-result.hpp>
#include <gyoji-frontend/parser.hpp>
#include <gyoji-frontend/type-resolver.hpp>
#include <gyoji-frontend/function-resolver.hpp>
#include <gyoji-frontend/lex-context.hpp>
#undef _GYOJI_INTERNAL



//! All aspects of the Gyoji language belong in this namespace. 
/*!
 * The Gyoji namespace contains the main components of the
 * compiler.
 */
namespace Gyoji {
    
};

/*!
 *  \addtogroup Frontend
 *  @{
 */
//! This is the front-end to the Gyoji parser.
/*!
 * This is the language front-end.
 * The purpose is to take an input
 * byte-stream and assemble an immutable
 * parse tree.
 */

namespace Gyoji::frontend {
};

/*! @} End of Doxygen Groups*/
