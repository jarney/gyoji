#pragma once

#include <string>
#include <vector>
#include <memory>

#include <jlang-misc/pointers.hpp>
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
namespace JLang::errors {

  class Errors;
  class Error;
  class ErrorMessage;
  

  class ErrorMessage {
  public:
    ErrorMessage(std::vector<std::pair<size_t, std::string>> _context,
                 size_t _line,
                 size_t _column,
                 std::string _errormsg
                 );
    ~ErrorMessage();
    void print();
    const std::vector<std::pair<size_t, std::string>> & get_context() const;
    size_t get_line() const;
    size_t get_column() const;
    const std::string & get_message() const;
  private:
    std::vector<std::pair<size_t, std::string>> context;
    size_t line;
    size_t column;
    std::string errormsg;
  };
  
  class Error {
  public:
    Error(std::string _error_message);
    ~Error();
    void add_message(
                     std::vector<std::pair<size_t, std::string>> context,
                     size_t lineno,
                     size_t colno,
                     std::string errormsg);
    void print();
    size_t size() const;
    const ErrorMessage & get(size_t n) const;
  private:
    std::vector<::JLang::owned<ErrorMessage>> messages;
    std::string error_message;
  };
  
  class Errors {
  public:
    Errors();
    ~Errors();
    void add_error(::JLang::owned<Error> error);
    void print() const;
    size_t size() const;
    const Error & get(size_t n) const;
  private:
    std::vector<::JLang::owned<Error>> errors;
  };
  
};
