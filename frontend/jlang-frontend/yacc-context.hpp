#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend {
  class Parser;
}

namespace JLang::frontend::yacc {
  class YaccParser;
  
  class YaccContext {
  public:
    YaccContext(
                JLang::frontend::namespaces::NamespaceContext & _namespace_context
                );
    ~YaccContext();
    const JLang::frontend::namespaces::NamespaceContext & get_namespace_context() const;
    
    const JLang::frontend::tree::TranslationUnit & get_translation_unit() const;
    void set_translation_unit(JLang::frontend::tree::TranslationUnit_owned_ptr );

    friend JLang::frontend::yacc::YaccParser;
    
    friend JLang::frontend::Parser;
  private:
    JLang::frontend::tree::TranslationUnit_owned_ptr translation_unit;
    JLang::frontend::namespaces::NamespaceContext& namespace_context;
  };  
};

