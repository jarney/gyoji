#pragma once

#include "jbackend.hpp"

/**
 * This is the identity transformation that should always
 * format the code exactly as it appeared in the input,
 * assuming that the input is a valid syntax tree.
 */
class JBackendFormatIdentity : public JBackend {
public:
  JBackendFormatIdentity();
  ~JBackendFormatIdentity();
  virtual int process(ASTNode::ptr file);
};
