#pragma once

#include <list>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include "ast.hpp"
#include "namespace.hpp"

typedef struct return_data_st {
  ASTNode::ptr parsed;
  NamespaceContext namespace_context;
} return_data_t;

