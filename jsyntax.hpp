#pragma once

#include <list>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include "ast.hpp"

typedef struct return_data_st {
  ASTNode::ptr parsed;
} return_data_t;

