#pragma once

#include <jlang-frontend/jsyntax.hpp>
namespace JLang::backend {
  using namespace JLang::frontend;
  
  class JBackend {
  public:
    JBackend();
    ~JBackend();
    virtual int process(const SyntaxNode & file) = 0;
  };
};
