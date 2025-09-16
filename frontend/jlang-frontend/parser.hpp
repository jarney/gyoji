#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend {
  
  // Last piece before we button up the
  // interface to the parser and document
  // the whole thing.
  class Parser {
  public:
    Parser(
           JLang::frontend::namespaces::NamespaceContext & _namespace_context);
    ~Parser();
    int parse(InputSource & _input_source);
    
    JLang::frontend::tree::TranslationUnit_owned_ptr get_translation_unit();
    const JLang::frontend::namespaces::NamespaceContext & get_namespace_context() const;
  private:
    JLang::frontend::yacc::YaccContext yacc_context;
  };

};
