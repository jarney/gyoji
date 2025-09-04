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

bool type_exists(std::string name);
void define_type(std::string name, ASTNode::ptr definition);

