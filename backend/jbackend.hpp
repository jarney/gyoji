#pragma once

#include <jlang-frontend/jsyntax.hpp>

class JBackend {
public:
  JBackend();
  ~JBackend();
  virtual int process(ASTNode::ptr file) = 0;
};

/*
*/
