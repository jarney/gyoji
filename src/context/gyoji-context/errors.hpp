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
#include <memory>

#include <gyoji-misc/pointers.hpp>
#include <gyoji-context/source-reference.hpp>

namespace Gyoji::context {
    
    class Errors;
    class Error;
    class ErrorMessage;
    
    /**
     * @brief Message about a specific location in the code.
     *
     * @details
     * An error message is a specific observation about a line of
     * code.  This consists of a message string and a SourceReference
     * pointing out the specific location that the message refers to.
     */
    class ErrorMessage {
    public:
	ErrorMessage(
	    const SourceReference & _src_ref,
	    std::string _errormsg
	    );
	~ErrorMessage();
	/**
	 * Prints the specific error along with the associated
	 * line of code and surrounding context.
	 */
	void print();
	/**
	 * Returns a SourceReference pointing to the specific
	 * location where the error occurred.
	 */
	const SourceReference & get_source_ref() const;
	/**
	 * Returns the specific message string for this message.
	 */
	const std::string & get_message() const;
	/**
	 * This is used to inject the surrounding lines of code
	 * where the message occurred so that errors can be easily seen
	 * in the surrounding context.
	 */
	void add_context(const std::vector<std::pair<size_t, std::string>> & _context);
	/**
	 * Returns the line number where the error message occurred.
	 */
	size_t get_line() const;
    private:
	std::vector<std::pair<size_t, std::string>> context;
	SourceReference src_ref;
	std::string errormsg;
    };

    typedef size_t ErrorId;
    
    /**
     * @brief An error reported during compilation.
     * @details
     * An error represents an observation about the
     * input file that makes it invalid for some reason.
     * Some errors represent invalid syntax such as misplacing
     * a semicolon.  Others represent semantics like
     * undeclared variables or variables used prior to
     * initialization.  In many cases, errors are
     * related to self-consistency and may need to
     * reference one or more source locations.  Each
     * separate 'message' represents something we want
     * to point out about a particular source location
     * through a SourcReference.
     */
    class Error {
    public:
	/**
	 * Creates a new error with a 'title' for
	 * the error indicating what the error is
	 * generally about.
	 */
	Error(std::string _error_title);
	/**
	 * Move along, nothing to see here.
	 */
	~Error();

	/**
	 * Return the unique ID of the error.
	 * Uniqueness of error IDs isn't established
	 * by this system, but each system that uses
	 * errors must 'namespace' the error codes
	 * carefully to avoid conflicts.
	 * <pre>
	 * 0x0000_0000 to 0x0000_ffff IO errors:
	 *                            Errors that pertain to pre-processing
	 *                            the input in the "CPP" layer or
	 *                            having to do with the compiler's ability
	 *                            to read input from a source.
	 *
	 * 0x0001_0000 to 0x0001_ffff Syntax errors:
	 *                            Errors that pertain to converting
	 *                            the source file into a structured
	 *                            tree.
	 *
	 * 0x0002_0000 to 0x0002_ffff Semantic/lowering errors
	 *                            related to converting the syntax
	 *                            tree into the MIR representation.
	 *
	 * 0x0003_0000 to 0x0003_ffff Analysis/static analysis errors
	 *                            Borrow checker, for example,
	 *                            and use-before-initialization
	 *                            would raise these errors.
	 *
	 * 0x0004_0000 to 0x0004_ffff Code generation errors
	 *                            These should almost always be compiler
	 *                            bugs because the prior layers should
	 *                            make it impossible to receive these.
	 * </pre>
	 */
	ErrorId get_id() const;
	
	/**
	 * This adds a message to the error pointing out
	 * the specific source location where the error occurred.
	 */
	void add_message(const SourceReference & _src_ref,
			 std::string _errormsg);
	/**
	 * This prints the error along with the messages
	 * and the source lines where the error occurred
	 * and a little marker indicating the specific location
	 * in that line where the error occurred.
	 */
	void print();
	/**
	 * This returns the number of messages associated
	 * with this error.
	 */
	size_t size() const;
	/**
	 * Returns a specific message in this error.
	 */
	const ErrorMessage & get(size_t n) const;
	/**
	 * Returns the list of messages.
	 */
	const std::vector<Gyoji::owned<ErrorMessage>> & get_messages() const;
    private:
	std::vector<Gyoji::owned<ErrorMessage>> messages;
	std::string error_title;
    };
    
    class TokenStream;

    /**
     * @brief Container for errors reported.
     *
     * @details
     * The errors object is a container for any errors
     * reported during the lex, parse, lowering, analysis,
     * or code-generation layers.  Each error is associated with
     * one oe more messages with each message referencing the
     * specific SourceReference where the error occurred.
     */
    class Errors {
    public:
	/**
	 * References the token stream in order to allow
	 * the error reporting to gather the surrounding
	 * source lines and provide additional context to
	 * reported errors.
	 */
	Errors(TokenStream & _token_stream);
	/**
	 * Move along, nothing to see here.
	 */
	~Errors();
	/**
	 * Adds an error to the error list.
	 */
	void add_error(Gyoji::owned<Error> error);
	/**
	 * Adds a 'simple' error with only one message
	 * and one source location to the error list.
	 * This is used for things like 'variable not declared'
	 * where the error only needs to reference a single
	 * source location and say something about it.
	 */
	void add_simple_error(
	    const SourceReference & src_ref,
	    std::string _error_title,
	    std::string _error_message
	    );
	/**
	 * This is the main mechanism where errors are
	 * reported in a human-readable way, pointing out the
	 * context and specific location of the error.
	 */
	void print() const;
	/**
	 * This returns the number of errors reported so far.
	 * We can remove this once get_errors_of_type() works.
	 */
	size_t size() const;
	/**
	 * This returns a reference to a single specific error.
	 * We can remove this once get_errors_of_type() works
	 * and we have errors associated by type to go through.
	 */
	const Error & get(size_t n) const;

	/**
	 * Returns true if there is at least one error reported.
	 */
	bool has_errors() const;
	
	/**
	 * Returns true if there is an error of the
	 * given type.
	 */
	bool has_errors_of_type(ErrorId error_type) const;
	
	/**
	 * This returns all of the errors of a particular type.
	 */
	const std::vector<Error*> & get_errors_of_type(ErrorId error_type) const;
	
    private:
	std::vector<Gyoji::owned<Error>> errors;
	std::map<ErrorId, std::vector<Error*>> errors_by_id;
	const TokenStream & token_stream;
    };
    
};
