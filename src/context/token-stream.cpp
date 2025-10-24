/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <gyoji-context/token-stream.hpp>

using namespace Gyoji::context;

static const std::string internal_filename("internal");
static const SourceReference zero_source_ref(internal_filename, 1, 0, 0);

const SourceReference &
TokenStream::get_zero_source_ref()
{
    return zero_source_ref;
}

TokenStream::TokenStream()
{}

TokenStream::~TokenStream()
{}

const std::vector<Gyoji::owned<Token>> &
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
    line_start = (unsigned long)std::max((long)line_start, (long)0);
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
    TokenID _typestr,
    std::string _value,
    const std::string & _filename,
    size_t _line,
    size_t _column
    )
{
    Gyoji::owned<Token> token = Gyoji::owned_new<Token>(_typestr, _value, _filename, _line, _column, _value.size());
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
    TokenID _typestr,
    std::string _value,
    const std::string & _filename,
    size_t _line,
    size_t _column,
    size_t _length
    )
    : typestr(_typestr)
    , value(_value)
    , src_ref(_filename, _line, _column, _length)
{}

Token::~Token()
{}

static std::string no_type_string("No-type");

const TokenID &
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
const SourceReference &
Token::get_source_ref() const
{ return src_ref; }
