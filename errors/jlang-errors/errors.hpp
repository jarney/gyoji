#pragma once

#include <string>
#include <vector>

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

  class Errors {
  public:
    Errors();
    ~Errors();
    void add_error(std::string errmsg);
  private:
    std::vector<std::string> errors;
  };
  
};
