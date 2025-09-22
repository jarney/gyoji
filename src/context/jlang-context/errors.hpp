#pragma once

#include <string>
#include <vector>
#include <memory>

#include <jlang-misc/pointers.hpp>
#include <jlang-context/source-reference.hpp>
/*!
 * The errors namespace contains code related
 * to reporting errors about the source code.
 * Errors may arise in any stage of the pipeline
 * processing starting at the lexical analysis
 * stage through the syntax, type system, and
 * code generation levels.  This class generalizes
 * all error handling at all levels so that each
 * level can introduce errors that can be
 * reported with a common format so that it
 * can be reported with the full context of
 * where the error originated.
 */
namespace JLang::context {

  class Errors;
  class Error;
  class ErrorMessage;
  

  class ErrorMessage {
  public:
    ErrorMessage(const SourceReference & _src_ref,
                 std::string _errormsg
                 );
    ~ErrorMessage();
    void print();
    const std::vector<std::pair<size_t, std::string>> & get_context() const;
    const SourceReference & get_source_ref() const;
    const std::string & get_message() const;
    void add_context(const std::vector<std::pair<size_t, std::string>> & _context);
    size_t get_line() const;
    size_t get_column() const;
    std::string get_filename() const;
  private:
    std::vector<std::pair<size_t, std::string>> context;
    SourceReference src_ref;
    std::string errormsg;
  };

  class Error {
  public:
    Error(std::string _error_message);
    ~Error();
    void add_message(const SourceReference & _src_ref,
                     std::string _errormsg);
    void print();
    size_t size() const;
    const ErrorMessage & get(size_t n) const;
    const std::vector<JLang::owned<ErrorMessage>> & get_messages() const;
  private:
    std::vector<JLang::owned<ErrorMessage>> messages;
    std::string error_message;
  };

  class TokenStream;
  
  class Errors {
  public:
    Errors(TokenStream & _token_stream);
    ~Errors();
    void add_error(JLang::owned<Error> error);
    void add_simple_error(
                          const SourceReference & src_ref,
                          std::string _error_title,
                          std::string _error_message
                          );
    void print() const;
    size_t size() const;
    const Error & get(size_t n) const;
  private:
    std::vector<JLang::owned<Error>> errors;
    const TokenStream & token_stream;
  };
  
};
