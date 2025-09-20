#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend {
  class Token;
  class TokenStream;

  /**
   * A token represents the result of the lexical
   * analysis and contains the line number and column
   * number of where the token was found as well as
   * the type of token and the literal value of the
   * token from the input.  Some tokens participate
   * in the grammar such as language keywords
   * while others represent whitespace or comments.
   */
  class Token {
  public:
    /**
     * Creates a new token of the given type
     * and value with the line number and column
     * number where it was found in the input
     * stream.
     */
    Token(
          std::string _typestr,
          std::string _value,
          size_t _line,
          size_t _column
          );
    /**
     * Destructor, nothing fancy.
     */
    ~Token();
    /**
     * This returns the type of the token.
     * In most cases, this will be the %token
     * symbol from the grammar definition
     * but in some cases may refer to a
     * whitespace or comment directly from
     * the input and bypassing the grammar
     * entirely.
     */
    const std::string & get_type() const;
    /**
     * This is the literal value that was
     * found in the input stream and matched
     * one of the lexical rules.
     */
    const std::string & get_value() const;
    /**
     * This is the line number where the token
     * was found in the input.
     */
    const size_t get_line() const;
    /**
     * This is the column number where the
     * token was found.
     */
    const size_t get_column() const;
    
    friend TokenStream;
  private:
    /**
     * This is used internally by the lexer
     * to append data to a token.
     */
    void append(std::string & value);
    std::string typestr;
    std::string value;
    size_t line;
    size_t column;
  };

  /**
   * The token stream represents the list of all tokens that were encountered
   * while parsing the input file.  Each token corresponds to a match rule
   * in the lexical analysis stage (jlang.l).  The tokens also contain metadata
   * indicating the location where the token was found as well as the rule
   * that matched the token.  The token stream can be used to exactly reproduce
   * the input and is useful in constructing structured error messages where
   * it is useful to have some context of the original source file.
   */
  class TokenStream {
  public:
    /**
     * Creates an empty token stream to hold token data.
     */
    TokenStream();
    /**
     * Destructor, nothing fancy.
     */
    ~TokenStream();

    /**
     * This returns a list of all of the tokens found during
     * the parse.  This is returned as an immutable list
     * of pointers owned by the token stream.
     */
    const std::vector<::JLang::owned<Token>> & get_tokens() const;

    /**
     * This returns the exact text of a single line of source-data.
     * This is useful in constructing the context for structured
     * error messages.
     */
    std::string get_line(size_t _line) const;

    /**
     * This is used by the lexical analysis stage (jlang.l) when it
     * matches a rule for a token.  It sets the token type (match rule)
     * as well as that data that matched the token and the source
     * line number and column where the token was found.
     */
    const Token & add_token(std::string _typestr,
                            std::string _value,
                            size_t _line,
                            size_t _column
                            );
    
    /**
     * This returns a list of lines from the source file
     * starting with line_start and ending with line_end (inclusive).
     * This is useful in providing context to structured errors.
     * @param line_start Start line number to retrieve.
     * @param line_end Last line number to retrieve.
     * @return This returns a pair of line number and line text for the matched lines from the source file.
     */
    std::vector<std::pair<size_t, std::string>> context(size_t line_start, size_t line_end) const;

    /**
     * This method is used to append a value to the
     * most recently added token.  Note that this
     * has no effect if no tokens have been added yet.
     * This is used internally for parsing things like
     * multi-line comments where a single rule may not
     * match the entire token, so it is broken up
     * into several match rules such as C-style
     * multi-line comments.
     */
    void append_token(std::string _value);
  private:
    
    std::vector<::JLang::owned<Token>> tokens;
    std::vector<Token*> empty_list;
    std::map<size_t, std::vector<Token*>> tokens_by_lineno;
  };
};
