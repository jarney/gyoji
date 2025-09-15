#pragma once

#include <jlang-frontend/jsyntax.hpp>

/*!
 *  \addtogroup Backend
 *  @{
 */
//! Headline News for the back-end.
/*!
 * The back-end is a set of functions
 * that process the input.
 */
namespace JLang::backend {
  using namespace JLang::frontend;
  
  class JBackend {
  public:
    JBackend();
    ~JBackend();
    virtual int process(const SyntaxNode & file) = 0;
  };
};

/*! @} End of Doxygen Groups*/
