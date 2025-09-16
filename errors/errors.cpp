#include <jlang-errors/errors.hpp>

using namespace JLang::errors;

Errors::Errors()
{}
Errors::~Errors()
{}

    // What we really want here is enough context (from lineno)
    // to recall the surrounding code so that it can be nicely
    // formatted and we can report exactly where the error
    // occurred and ----^ point to the exact spot where something
    // bad happened.
    // In the simplest case, suppose we have a map of line number to
    // line of code.  Then we can simply do a lex_context->lines[lex_context->lineno]
    // and use that to get the bad line of code.  Then if we also have
    // a TERMINAL or some other syntax element, we can use that to identify
    // the column inside that line to print the error line.
#if 0
void
Errors::add_error(
                  std::vector<std::string> surrounding_context,
                  int lineno,
                  int colno,
                  std::string errormsg)
{
  // Print the context before:
  // {
  //     some_valid_statement();
  // Then, print the offending line.
  //     a % "234";
  // Then point out exactly where we went wrong:
  //     a % "234";
  // --------^ Wrong type.

  // An error for the borrow checker may include several
  // sites and contexts.  For example:
  // * Point where borrow occurred.
  // * Point where reference occurred.
  // * Point where reference is invalidated.
  
  // In order to accommodate such a structure, we would want
  // an error to be reported as:
  Error error("Invalid borrow used");
  error.add_message(<...>, 12, 4, "Borrow occurred here");
  error.add_message(<....>, 23, 9, "Reference used here");
  error.add_message(<....>, 18, 9, "Invalidated here");
}
#endif

Error::Error(std::string _error_message)
  : error_message(_error_message)
{}
Error::~Error()
{}
void
Error::print()
{
  fprintf(stderr, "Error: %s\n", error_message.c_str());
  for (const std::unique_ptr<ErrorMessage> & msg : messages) {
    msg->print();
  }
}

void
Error::add_message(std::vector<std::pair<int, std::string>> context,
                    int lineno,
                    int colno,
                    std::string errormsg)
{
  std::unique_ptr<ErrorMessage> message = std::make_unique<ErrorMessage>(
                                                                         context,
                                                                         lineno,
                                                                         colno,
                                                                         errormsg
                                                                         );
  messages.push_back(std::move(message));
}

ErrorMessage::ErrorMessage(std::vector<std::pair<int, std::string>> _context,
                           int _lineno,
                           int _colno,
                           std::string _errormsg
                           )
  : context(_context)
  , lineno(_lineno)
  , colno(_colno)
  , errormsg(_errormsg)
{}

ErrorMessage::~ErrorMessage()
{}

void
ErrorMessage::print()
{
  for (const std::pair<int, std::string> & line : context) {
    fprintf(stderr, "%4d: %s\n", line.first, line.second.c_str());
    if (lineno == line.first) {
      printf("Error on this line %s\n", errormsg.c_str());
    }
  }
}


void
Errors::add_error(std::unique_ptr<Error> error)
{
  errors.push_back(std::move(error));
}
