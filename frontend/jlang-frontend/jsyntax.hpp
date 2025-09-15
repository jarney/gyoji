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
