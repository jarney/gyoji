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
#pragma once
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <gyoji-misc/pointers.hpp>
#include <gyoji-context/source-reference.hpp>

namespace Gyoji::context {
    class Token;
    class TokenStream;

    typedef unsigned int TokenID;
    
    /**
     * @brief Represents a token read from the input stream.
     *
     * @details
     * A token represents the result of the lexical
     * analysis and contains the line number and column
     * number of where the token was found as well as
     * the type of token and the literal value of the
     * token from the input.  Some tokens participate
     * in the grammar such as language keywords
     * while others represent whitespace or comments.
     *
     * While it might seem that this belongs in the lex/parse
     * code of the front-end, this token is less concrete in the
     * sense that it is actually agnostic of the way the
     * data is broken up and parsed.  This token stream can actually
     * be re-used for multiple different lexical and syntactical structures.
     *
     * It also carries the source location where the token was read
     * from so that its source-reference can be used to derive
     * and report the location of any errors.  It is declared
     * in the compiler context because the token stream must
     * have a lifetime at least as long as the error reporting
     * system because the errors hold references to the tokens
     * and their source references.
     */
    class Token {
    public:
	/**
	 * Creates a new token of the given type
	 * and value with the line number and column
	 * number where it was found in the input
	 * stream.
	 */
	Token(
	    TokenID _typestr,
	    std::string _value,
	    const std::string & _filename,
	    size_t _line,
	    size_t _column,
	    size_t _length
	    );
	/**
	 * Destructor, nothing fancy.
	 */
	~Token();
	/**
	 * This returns the type of the token.
	 * In most cases, this will be the %token
	 * symbol from the grammar definition
	 * but in some cases may refer to a
	 * whitespace or comment directly from
	 * the input and bypassing the grammar
	 * entirely.
	 */
	const TokenID & get_type() const;
	/**
	 * This is the literal value that was
	 * found in the input stream and matched
	 * one of the lexical rules.
	 */
	const std::string & get_value() const;

	const SourceReference & get_source_ref() const;
	
	friend TokenStream;
    private:
	/**
	 * This is used internally by the lexer
	 * to append data to a token.
	 */
	void append(std::string & value);
	TokenID typestr;
	std::string value;
	SourceReference src_ref;
    };

    /**
     * @brief Stream of tokens read by the parser to provide context
     *        for errors.
     *
     * @details
     *
     * The token stream represents the list of all tokens that were encountered
     * while parsing the input file.  Each token corresponds to a match rule
     * in the lexical analysis stage (gyoji.l).  The tokens also contain metadata
     * indicating the location where the token was found as well as the rule
     * that matched the token.  The token stream can be used to exactly reproduce
     * the input and is useful in constructing structured error messages where
     * it is useful to have some context of the original source file.
     */
    class TokenStream {
    public:
	/**
	 * Creates an empty token stream to hold token data.
	 */
	TokenStream();
	/**
	 * Destructor, nothing fancy.
	 */
	~TokenStream();
	
	/**
	 * This returns a list of all of the tokens found during
	 * the parse.  This is returned as an immutable list
	 * of pointers owned by the token stream.
	 */
	const std::vector<Gyoji::owned<Token>> & get_tokens() const;
	
	/**
	 * Returns the most recent source reference found.
	 * If no prior source reference was found, this must
	 * be the first token in the file and we will return
	 * the most recent one.
	 */
	const SourceReference & get_current_source_ref() const;
	
	/**
	 * This returns the exact text of a single line of source-data.
	 * This is useful in constructing the context for structured
	 * error messages.
	 */
	std::string get_line(size_t _line) const;
	
	/**
	 * This is used by the lexical analysis stage (gyoji.l) when it
	 * matches a rule for a token.  It sets the token type (match rule)
	 * as well as that data that matched the token and the source
	 * line number and column where the token was found.
	 */
	const Token & add_token(
	    TokenID _typestr,
	    std::string _value,
	    const std::string & _filename,
	    size_t _line,
	    size_t _column
	    );
	
	/**
	 * This returns a list of lines from the source file
	 * starting with line_start and ending with line_end (inclusive).
	 * This is useful in providing context to structured errors.
	 * @param line_start Start line number to retrieve.
	 * @param line_end Last line number to retrieve.
	 * @return This returns a pair of line number and line text for the matched lines from the source file.
	 */
	std::vector<std::pair<size_t, std::string>> context(size_t line_start, size_t line_end) const;
	
	/**
	 * This method is used to append a value to the
	 * most recently added token.  Note that this
	 * has no effect if no tokens have been added yet.
	 * This is used internally for parsing things like
	 * multi-line comments where a single rule may not
	 * match the entire token, so it is broken up
	 * into several match rules such as C-style
	 * multi-line comments.
	 */
	void append_token(std::string _value);

	static const SourceReference & get_zero_source_ref();

    private:
	
	std::vector<Gyoji::owned<Token>> tokens;
	std::map<size_t, std::vector<Token*>> tokens_by_lineno;
    };
};
