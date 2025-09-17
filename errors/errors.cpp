#include <jlang-errors/errors.hpp>

using namespace JLang::errors;

//////////////////////////////////////////////////
Errors::Errors()
{}
Errors::~Errors()
{}

void
Errors::print() const
{
  for (const auto & error : errors) {
    error->print();
  }
}

void
Errors::add_error(std::unique_ptr<Error> error)
{
  errors.push_back(std::move(error));
}
//////////////////////////////////////////////////
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
Error::add_message(std::vector<std::pair<size_t, std::string>> context,
                    size_t lineno,
                    size_t colno,
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

//////////////////////////////////////////////////
ErrorMessage::ErrorMessage(std::vector<std::pair<size_t, std::string>> _context,
                           size_t _lineno,
                           size_t _colno,
                           std::string _errormsg
                           )
  : context(_context)
  , lineno(_lineno)
  , colno(_colno)
  , errormsg(_errormsg)
{}

ErrorMessage::~ErrorMessage()
{}

const std::vector<std::pair<size_t, std::string>> &
ErrorMessage::get_context() const
{ return context; }

size_t
ErrorMessage::get_line() const
{ return lineno; }

size_t
ErrorMessage::get_column() const
{ return colno; }

const std::string &
ErrorMessage::get_message() const
{ return errormsg; }


void
ErrorMessage::print()
{
  for (const std::pair<size_t, std::string> & line : context) {
    fprintf(stderr, "%4ld: %s", line.first, line.second.c_str());
    if (line.second.at(line.second.size()-1) != '\n') {
      fprintf(stderr, "\n");
    }
    if (lineno == line.first) {
      printf("Error on this line %s\n", errormsg.c_str());
    }
  }
}


