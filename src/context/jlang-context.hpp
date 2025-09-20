#pragma once

#include <jlang-context/errors.hpp>
#include <jlang-context/token-stream.hpp>

namespace JLang::context {
  class CompilerContext {
  public:
    CompilerContext();
    ~CompilerContext();

    Errors & get_errors() const;
    TokenStream & get_token_stream() const;
  private:
    JLang::owned<Errors> errors;
    JLang::owned<TokenStream> token_stream;
  };
};
