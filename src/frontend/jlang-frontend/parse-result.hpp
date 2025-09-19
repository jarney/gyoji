#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend::yacc {
  class YaccParser;
  class LexContext;
}

/*!
 *  \addtogroup Frontend
 *  @{
 */
namespace JLang::frontend {
  class Parser;
  class TokenStream;

  /**
   * This class represents the result of parsing a source-file.
   * This class holds all of the results relevant to that parse.
   *
   * The syntax tree is held by the "translation_unit" which represents
   * the parse-tree of any valid parse of the input.
   *
   * In the event that some or all of the input could not be parsed
   * correctly, the "errors" object contains detailed information
   * about exactly what is wrong with the parse including any errors
   * and descriptions as well as context information surrounding the site
   * where the parse failed.
   *
   * In addition, the source-file may make use of namespaces to define
   * types, classes, functions inside them and organize them in
   * a heirarchy.  The "namespaces" object returns information about
   * the types and namespaces they are defined inside of including
   * the fully-qualified type names, for example, a primitive type
   * will have name "::u32", for example, whereas another type
   * may be defined in the "foo" namespace as "::foo::bar".
   *
   * Finally, the "token_stream" contains the list of tokens
   * parsed by the lexer including verbatim copies of the input.
   * This data may be useful for code-formatters or producing rich
   * error messages with the full context of the input.
   */
  class ParseResult {
  public:
    /**
     * The constructor requires the caller to pass a namespace context
     * in order to define the built-in primitive types.
     * 
     * This is important because the primitive types are generally governed
     * by the back-end code-generation layer which must inform the initial
     * set of primitive types available.
     */
    ParseResult(
                ::JLang::owned<JLang::frontend::namespaces::NamespaceContext> _namespace_context
                );
    /**
     * Destructor, nothing special.
     */
    ~ParseResult();

    /**
     * This returns a reference to the namespace context containing
     * the namespaces defined by the source-file and the types and
     * classes defined in them.  Note that this reference may only
     * be accessed for as long as this (ParseResult) object is in scope
     * since the namespace context is owned by this object.
     */
    const JLang::frontend::namespaces::NamespaceContext & get_namespace_context() const;

    /**
     * This returns true if any parse errors occurred during the parse of the
     * source-file.
     */
    bool has_errors() const;
    
    /**
     * This returns an object containing the details of any parse errors
     * that occurred during the parse.
     */
    JLang::errors::Errors & get_errors() const;

    /**
     * This returns true if the parse resulted in a valid TranslationUnit.
     * That is to say, if the parse was able to parse correctly or recover
     * from any recoverable errors and produce a parse tree.  Note that
     * if the has_errors() returns true, then the resulting parse tree
     * may be inaccurate or incomplete.
     */
    bool has_translation_unit() const;

    /**
     * If the parse resulted in a valid parse, then the translation unit
     * returns the strongly-typed parse tree for the source-file.  This
     * can be used for multiple purposes including to reproduce the input
     * either identically or as a "pretty print" version.  It can also be
     * used to drive the intermediate representation and code-generation
     * in order to compile output.  The front-end does not perform any of those
     * tasks, but the parse tree generated here is a faithful and strongly-typed
     * representation of the input.
     */
    const JLang::frontend::tree::TranslationUnit & get_translation_unit() const;

    /**
     * This returns the token stream associated with the parse.
     * The token stream is the series of tokens produced by the
     * lexical analysis (jlang.l) and represents the input data
     * organized as tokens.  This token stream can be used to
     * reconstruct all of the input data, but also allows structured
     * access to it for the purpose of producing structured error messages.
     */
    const JLang::frontend::TokenStream & get_token_stream() const;

    friend JLang::frontend::yacc::YaccParser;
    friend JLang::frontend::yacc::LexContext;
    friend JLang::frontend::Parser;

  private:
    /**
     * This is used internally by the YACC grammar to return the parse tree.
     */
    void set_translation_unit(::JLang::owned<JLang::frontend::tree::TranslationUnit> tu);
    
    ::JLang::owned<JLang::frontend::namespaces::NamespaceContext> namespace_context;
    
    ::JLang::owned<JLang::errors::Errors> errors;
    
    ::JLang::owned<JLang::frontend::TokenStream> token_stream;
    
    ::JLang::owned<JLang::frontend::tree::TranslationUnit> translation_unit;

  };

};


/*! @} End of Doxygen Groups*/
