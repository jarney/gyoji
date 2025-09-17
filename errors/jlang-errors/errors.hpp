#pragma once

#include <string>
#include <vector>
#include <memory>

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


  class ErrorMessage {
  public:
    ErrorMessage(std::vector<std::pair<size_t, std::string>> _context,
                 size_t _lineno,
                 size_t _colno,
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
    size_t lineno;
    size_t colno;
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
  private:
    std::vector<std::unique_ptr<ErrorMessage>> messages;
    std::string error_message;
  };
  
  class Errors {
  public:
    Errors();
    ~Errors();
    void add_error(std::unique_ptr<Error> error);
    void print() const;
  private:
    std::vector<std::unique_ptr<Error>> errors;
  };
  
};
