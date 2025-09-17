#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

#include <jlang-frontend/input-source.hpp>

namespace JLang::frontend::yacc {
  class LexContext {
  public:
    LexContext(JLang::frontend::namespaces::NamespaceContext &_namespace_context,
               JLang::frontend::TokenStream &_token_stream,
               InputSource &_input_source);
    ~LexContext();
    JLang::frontend::namespaces::NamespaceContext& namespace_context;
    InputSource & input_source;
    JLang::frontend::TokenStream & token_stream;
    int lineno;
  };
};
