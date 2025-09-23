#include <jlang-context/token-stream.hpp>

using namespace JLang::context;

static const SourceReference zero_source_ref("", 1, 0);

TokenStream::TokenStream()
{}

TokenStream::~TokenStream()
{}

const std::vector<JLang::owned<Token>> &
TokenStream::get_tokens() const
{
  return tokens;
}

/**
 * Returns the most recent source reference found.
 * If no prior source reference was found, this must
 * be the first token in the file and we will return
 * the most recent one.
 */
const SourceReference &
TokenStream::get_current_source_ref() const
{
    if (tokens.size() == 0) {
	return zero_source_ref;
    }
    return tokens.back()->get_source_ref();
}


std::string TokenStream::get_line(size_t _line) const
{
    std::string msg;
    auto it = tokens_by_lineno.find(_line);
    if (it == tokens_by_lineno.end()) {
	return msg;
    }
    const std::vector<Token *> & tokens = it->second;
    for (Token* token : tokens) {
	msg += token->get_value();
    }
    return msg;
}

std::vector<std::pair<size_t, std::string>>
TokenStream::context(size_t line_start, size_t line_end) const
{
    std::vector<std::pair<size_t, std::string>> ret;
    line_start = std::max(line_start, (size_t)0);
    if (line_end < line_start) {
	return ret;
    }
    for (size_t i = line_start; i <= line_end; i++) {
	std::string msg(get_line(i));
	ret.push_back(std::pair<size_t, std::string>((size_t)i, msg));
    }
    return ret;
}

const Token &
TokenStream::add_token(
    std::string _typestr,
    std::string _value,
    std::string _filename,
    size_t _line,
    size_t _column
    )
{
    JLang::owned<Token> token = std::make_unique<Token>(_typestr, _value, _filename, _line, _column);
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
    std::string _filename,
    size_t _line,
    size_t _column
    )
    : typestr(_typestr)
    , value(_value)
    , src_ref(_filename, _line, _column)
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
{ return src_ref.get_line(); }

const size_t
Token::get_column() const
{ return src_ref.get_column(); }

const SourceReference &
Token::get_source_ref() const
{ return src_ref; }

