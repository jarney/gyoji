#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend {
  class TokenStream {
  public:
    TokenStream();
    ~TokenStream();
    void print_from_lex() const;
    void print_from_yacc() const;
    void print_from_main() const;
  };  
};
