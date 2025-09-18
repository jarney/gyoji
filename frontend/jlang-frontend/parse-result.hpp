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
  typedef std::unique_ptr<TokenStream> TokenStream_owned_ptr;
  
  class ParseResult {
  public:
    ParseResult(
                JLang::frontend::namespaces::NamespaceContext_owned_ptr _namespace_context
                );
    ~ParseResult();
    const JLang::frontend::namespaces::NamespaceContext & get_namespace_context() const;

    bool has_errors() const;
    const JLang::errors::Errors & get_errors() const;

    bool has_translation_unit() const;
    const JLang::frontend::tree::TranslationUnit & get_translation_unit() const;
    
    void set_translation_unit(JLang::frontend::tree::TranslationUnit_owned_ptr );

    const JLang::frontend::TokenStream & get_token_stream() const;

    friend JLang::frontend::yacc::YaccParser;
    friend JLang::frontend::yacc::LexContext;
    friend JLang::frontend::Parser;

  private:
    JLang::frontend::namespaces::NamespaceContext_owned_ptr namespace_context;
    
    JLang::errors::Errors_owned_ptr errors;
    
    JLang::frontend::TokenStream_owned_ptr token_stream;
    
    JLang::frontend::tree::TranslationUnit_owned_ptr translation_unit;

  };

  typedef std::unique_ptr<ParseResult> ParseResult_owned_ptr;
};

