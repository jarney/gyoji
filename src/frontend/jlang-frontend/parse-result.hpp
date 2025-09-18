#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend::yacc {
  class YaccParser;
  class LexContext;
}

namespace JLang::frontend {
  class Parser;
  class TokenStream;
  
  class ParseResult {
  public:
    ParseResult(
                ::JLang::owned<JLang::frontend::namespaces::NamespaceContext> _namespace_context
                );
    ~ParseResult();
    const JLang::frontend::namespaces::NamespaceContext & get_namespace_context() const;

    bool has_errors() const;
    const JLang::errors::Errors & get_errors() const;

    bool has_translation_unit() const;
    const JLang::frontend::tree::TranslationUnit & get_translation_unit() const;
    
    void set_translation_unit(::JLang::owned<JLang::frontend::tree::TranslationUnit> );

    const JLang::frontend::TokenStream & get_token_stream() const;

    friend JLang::frontend::yacc::YaccParser;
    friend JLang::frontend::yacc::LexContext;
    friend JLang::frontend::Parser;

  private:
    ::JLang::owned<JLang::frontend::namespaces::NamespaceContext> namespace_context;
    
    ::JLang::owned<JLang::errors::Errors> errors;
    
    ::JLang::owned<JLang::frontend::TokenStream> token_stream;
    
    ::JLang::owned<JLang::frontend::tree::TranslationUnit> translation_unit;

  };

};

