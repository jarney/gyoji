#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend {
  class Token;
  class TokenStream;
  typedef std::unique_ptr<Token> Token_owned_ptr;
  typedef std::unique_ptr<TokenStream> TokenStream_owned_ptr;

  class Token {
  public:
    Token(
          std::string _typestr,
          std::string _value,
          size_t _line,
          size_t _column
          );
    ~Token();
    const std::string & get_type() const;
    const std::string & get_value() const;
    const size_t get_line() const;
    const size_t get_column() const;
    void append(std::string & value);
  private:
      std::string typestr;
      std::string value;
      size_t line;
      size_t column;
  };
  
  class TokenStream {
  public:
    TokenStream();
    ~TokenStream();
    const std::vector<Token_owned_ptr> & get_tokens() const;
    std::string get_line(size_t _line) const;
    const Token & add_token(std::string _typestr,
                            std::string _value,
                            size_t _line,
                            size_t _column
                            );
    void append_token(std::string _value);
    
    /**
     * Provide the context of input lines
     * starting with line_start and ending with line_end.
     * Assumes that line_start < line_end and re-produces
     * the input source from the token stream so that
     * it can be used in error messages.
     */
    std::vector<std::pair<size_t, std::string>> context(size_t line_start, size_t line_end) const;
  private:
    std::vector<Token_owned_ptr> tokens;
    std::vector<Token*> empty_list;
    std::map<size_t, std::vector<Token*>> tokens_by_lineno;
  };
};
