#include <jlang-context/errors.hpp>

using namespace JLang::context;

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
Errors::add_error(::JLang::owned<Error> error)
{
  errors.push_back(std::move(error));
}

size_t
Errors::size() const
{
  return errors.size();
}

const Error &
Errors::get(size_t n) const
{
  return *errors.at(n);
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
  for (const ::JLang::owned<ErrorMessage> & msg : messages) {
    msg->print();
  }
}

void
Error::add_message(std::vector<std::pair<size_t, std::string>> _context,
                    size_t _line,
                    size_t _column,
                    std::string _errormsg)
{
  ::JLang::owned<ErrorMessage> message = std::make_unique<ErrorMessage>(
                                                                  _context,
                                                                  _line,
                                                                  _column,
                                                                  _errormsg
                                                                         );
  messages.push_back(std::move(message));
}

size_t
Error::size() const
{ return messages.size(); }

const ErrorMessage &
Error::get(size_t n) const
{ return *messages.at(n); }

//////////////////////////////////////////////////
ErrorMessage::ErrorMessage(std::vector<std::pair<size_t, std::string>> _context,
                           size_t _line,
                           size_t _column,
                           std::string _errormsg
                           )
  : context(_context)
  , line(_line)
  , column(_column)
  , errormsg(_errormsg)
{}

ErrorMessage::~ErrorMessage()
{}

const std::vector<std::pair<size_t, std::string>> &
ErrorMessage::get_context() const
{ return context; }

size_t
ErrorMessage::get_line() const
{ return line; }

size_t
ErrorMessage::get_column() const
{ return column; }

const std::string &
ErrorMessage::get_message() const
{ return errormsg; }

// Case 1.
//    ^
//    |
//    +---------message goes here, wrapped
//              to indent level

// Case 2
//                                    ^
//                                    |
//  message goes here, wrapped -------+
//  to indent level

static std::string pad_string(size_t length)
{
  std::string prefix;
  for (int i = 0; i < length; i++) {
    prefix = prefix + std::string(" ");
  }
  return prefix;
}

static void draw_arrow(size_t column)
{
  std::string arrowhead_line("^");
  std::string pipe_line("|");
  std::string prefix = pad_string(column);
  arrowhead_line = prefix + arrowhead_line;
  pipe_line = prefix + pipe_line;
  fprintf(stderr, "%s\n", arrowhead_line.c_str());
  fprintf(stderr, "%s\n", pipe_line.c_str());
}

static std::string wrap_text(size_t max_width, std::string input)
{
  std::string wrapped;

  size_t linelen = 0;
  for (size_t i = 0; i < input.size(); i++) {
    char c = input[i];
    linelen++;
    if (isspace(c)) {
      if (linelen > max_width) {
        wrapped += '\n';
        linelen = 0;
      }
      else {
        wrapped += c;
      }
    }
    else {
      wrapped += c;
    }
  }
  
  return wrapped;
}

static std::string indent_text(size_t indent, std::string input)
{
  std::string wrapped;

  std::string pad = pad_string(indent);
  wrapped.append(pad);
  for (size_t i = 0; i < input.size(); i++) {
    char c = input[i];
    wrapped += c;
    if (c == '\n') {
      wrapped.append(pad);
    }
  }
  
  return wrapped;
}

void
ErrorMessage::print()
{
  for (const std::pair<size_t, std::string> & linepair : context) {
    fprintf(stderr, "%4ld: %s", linepair.first, linepair.second.c_str());
    if (linepair.second.size() > 0) {
      if (linepair.second.at(linepair.second.size()-1) != '\n') {
        fprintf(stderr, "\n");
      }
    }
    if (line == linepair.first) {
      draw_arrow(column+5);
      if (column < 40) {
        std::string wrapped = wrap_text(80-column, errormsg);
        std::string indented = indent_text(column+5, wrapped);
        printf("%s\n", indented.c_str());
      }
      else {
        std::string wrapped = wrap_text(column, errormsg);
        std::string indented = indent_text(5, wrapped);
        printf("%s\n", indented.c_str());
      }
    }
  }
}


