#pragma once

#include <string>
#include <vector>
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
	 */
	size_t size() const;
	/**
	 * This returns a reference to a single specific error.
	 */
	const Error & get(size_t n) const;
    private:
	std::vector<Gyoji::owned<Error>> errors;
	const TokenStream & token_stream;
    };
    
};
