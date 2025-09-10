#pragma once

#include "jsyntax.hpp"

class JBackend {
public:
  JBackend();
  ~JBackend();
  virtual int process(ASTNode::ptr file) = 0;
};

/*
*/
