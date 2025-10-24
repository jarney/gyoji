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
#include <gyoji-frontend/type-lowering.hpp>
#include <gyoji-frontend/function-lowering.hpp>
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
