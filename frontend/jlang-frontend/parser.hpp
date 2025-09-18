#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend {
  
  typedef std::unique_ptr<TokenStream> TokenStream_owned_ptr;
  
  // Last piece before we button up the
  // interface to the parser and document
  // the whole thing.
  class Parser {
  public:
    Parser(JLang::frontend::namespaces::NamespaceContext_owned_ptr _namespace_context);
    ~Parser();
    int parse(InputSource & _input_source);
    
    ParseResult_owned_ptr get_parse_result();
  private:
    ParseResult_owned_ptr yacc_context;
  };
};
