#pragma once

#include <list>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include "ast.hpp"
#include "namespace.hpp"

namespace JLang::frontend {
  typedef struct return_data_st {
    ASTNode::ptr translation_unit;
    NamespaceContext namespace_context;
  } return_data_t;  
}
