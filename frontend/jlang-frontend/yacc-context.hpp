#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include jsyntax.hpp instead."
#endif
#pragma once

namespace JLang::frontend::yacc {
  class YaccContext {
  public:
    YaccContext(
                JLang::frontend::namespaces::NamespaceContext & _namespace_context
                );
    ~YaccContext();
    JLang::frontend::namespaces::NamespaceContext & get_namespace_context();
    
    JLang::frontend::tree::TranslationUnit_owned_ptr get_translation_unit();
    void set_translation_unit(JLang::frontend::tree::TranslationUnit_owned_ptr );
  private:
    JLang::frontend::tree::TranslationUnit_owned_ptr translation_unit;
    JLang::frontend::namespaces::NamespaceContext namespace_context;
  };

};

