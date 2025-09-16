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
  typedef struct return_data_st {
    tree::TranslationUnit_owned_ptr translation_unit;
    namespaces::NamespaceContext namespace_context;
  } return_data_t;

};

/*! @} End of Doxygen Groups*/
