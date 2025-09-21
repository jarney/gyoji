#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

/*!
 *  \addtogroup Frontend
 *  @{
 */
namespace JLang::frontend {

  /**
   * This is the main interface to the front-end parsing
   * system.  It contains only static methods to invoke the
   * parser and retrieve the syntax tree.  No state is
   * held by this class and it exists only to organize
   * the namespace of the interface.
   */
  class Parser {
  public:
    /**
     * This is the main interface to the front-end for the
     * parser.  The purpose is to wrap all of the lex/parse/syntax tree
     * components of the front-end into a common and simple to use
     * interface.
     *
     * The parser is responsible for reading data from the input and
     * passing it to the tokenizer (Flex-based lexical analyzer) and
     * then to the parser (Bison-based LALR(1) parser).  From there,
     * the resulting data is built into a syntax tree and is available
     * in the parse result.
     *
     * Once the parse result is returned, the caller takes ownership
     * of all of the resulting data and the parser itself is free
     * to leave scope having done its job.
     *
     * @param _namespace_context This is the namespace context containing the initial set of
     *                           primitive types.
     *
     * @param _input_source      This is the source from which to read data.
     */
    static JLang::owned<ParseResult> parse(
                                           JLang::context::CompilerContext & _compiler_context,
                                           JLang::misc::InputSource & _input_source
                                           );

    /**
     * This function parses the input code and produces an MIR
     * representation of the result that can be passed directly
     * to the analysis and later, the code-generation systems
     * to generate code.  This is the ultimate result of
     * the front-end.  Any alternative front-end syntax layers
     * should also parse using the context and produce an MIR
     * as a result.  The caller will take ownership of the resulting
     * MIR and make it available to downstream consumers.
     */
    static JLang::owned<JLang::mir::MIR> parse_to_mir(
                                          JLang::context::CompilerContext & _compiler_context,
                                          JLang::misc::InputSource & _input_source
                                          );
  };

};

/*! @} End of Doxygen Groups*/
