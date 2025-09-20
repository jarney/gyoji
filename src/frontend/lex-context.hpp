#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend::yacc {
  class LexContext {
  public:
    LexContext(JLang::frontend::namespaces::NamespaceContext &_namespace_context,
               JLang::context::TokenStream &_token_stream,
               JLang::misc::InputSource &_input_source);
    ~LexContext();
    JLang::frontend::namespaces::NamespaceContext& namespace_context;
    JLang::misc::InputSource & input_source;
    JLang::context::TokenStream & token_stream;
    size_t line;
    size_t column;
  };
};
