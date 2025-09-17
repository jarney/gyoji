#include <jlang-frontend.hpp>

using namespace JLang::frontend;

TokenStream::TokenStream()
{}

TokenStream::~TokenStream()
{}

void
TokenStream::print_from_lex() const
{
  fprintf(stderr, "Printing from lexer\n");
}

void
TokenStream::print_from_yacc() const
{
  fprintf(stderr, "Printing from yacc\n");
}

void
TokenStream::print_from_main() const
{
  fprintf(stderr, "Printing from main\n");
}

const std::vector<Token_owned_ptr> &
TokenStream::get_tokens() const
{
  return tokens;
}

const std::vector<Token*> &
TokenStream::get_tokens_by_lineno(size_t _line) const
{
  auto it = tokens_by_lineno.find(_line);
  if (it == tokens_by_lineno.end()) {
    return empty_list;
  }
  return it->second;
}

const Token &
TokenStream::add_token(std::string _typestr,
                       std::string _value,
                       size_t _line,
                       size_t _column
                       )
{
  Token_owned_ptr token = std::make_unique<Token>(_typestr, _value, _line, _column);
  const Token & token_ref = *token;
  tokens_by_lineno[_line].push_back(token.get());
  tokens.push_back(std::move(token));
  return token_ref;
}

void
TokenStream::append_token(std::string _value)
{
  if (tokens.empty()) return;
  const auto & it = tokens.back();
  it->append(_value);
}

Token::Token(
          std::string _typestr,
          std::string _value,
          size_t _line,
          size_t _column
          )
  : typestr(_typestr)
  , value(_value)
  , line(_line)
  , column(_column)
{}

Token::~Token()
{}

const std::string &
Token::get_type() const
{ return typestr; }

const std::string &
Token::get_value() const
{ return value; }

void
Token::append(std::string & _value)
{
  value += _value;
}

const size_t
Token::get_line() const
{ return line; }

const size_t
Token::get_column() const
{ return column; }
