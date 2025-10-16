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

#include <gyoji-context/errors.hpp>
#include <gyoji-context/token-stream.hpp>
#include <gyoji-context/source-reference.hpp>

/**
 * @brief The context namespace deals with objects that
 *        should last the entire scope of compilation.
 *
 * @details
 * The compiler context namespace is a container for objects
 * that may be used at any stage throughout the compilation process
 * from initial ingestion to analysis and error reporting.
 */
namespace Gyoji::context {

    /**
     * @brief Compiler Context
     *
     * @details
     * The compiler context is an object that holds
     * the token stream used to tokenize the input
     * and also hold references to the source-file
     * that the data was read from.  In addition, it
     * holds the error reporting object which is the
     * main mechanism where errors during parse, lowering,
     * analysis, and code-generation are reported.
     */
    class CompilerContext {
    public:
	/**
	 * Constructs a context that can be used for
	 * compiling a source file.  The filename given
	 * here is used to provide context to the error system,
	 * but is not directly used in reading the file,
	 * only for producing context-driven error messages.
	 */
	CompilerContext(std::string _filename);
	~CompilerContext();

	/**
	 * This returns true if any errors were reported
	 * so far during the compilation process.  Presence of
	 * errors usually stops the next stage of compilation from
	 * proceeding because there may be errors that
	 * would make the next stage nonsense or inconsistent.
	 */
	bool has_errors() const;
	
	/**
	 * This returns the errors object that holds
	 * all of the errors reported so far.
	 */
	Errors & get_errors() const;

	/**
	 * This returns the token stream which is a container for the input
	 * as it was directly read from the input source file.  The token stream
	 * contains enough information to completely re-construct the input
	 * source file, so in that sense, there is no 'loss' of data
	 * as it is read into the token stream.
	 */
	TokenStream & get_token_stream() const;

	/**
	 * This returns the 'current' source file at this point
	 * in the compilation.  Note that some statements like the #line
	 * directive in the pre-processor can be used to declare
	 * the source location to be physically different than where
	 * the source is actually read from.  This is used for situations
	 * where code is generated, for example, and allows us to associate
	 * generated code with the source file/location where it was generated from.
	 */
	const std::string & get_filename() const;

	/**
	 * This allows us to declare that the source file is different than
	 * the actual physical source file we're reading from.  This is used
	 * in situations where code is generated and that tool would like to
	 * trace the generated code back to the location where it was generated
	 * from.  This allows errors to be traced all the way back to the 'original'
	 * location where they were generated from.
	 */
	void add_filename(const std::string & _filename);
    private:
	Gyoji::owned<Errors> errors;
	Gyoji::owned<TokenStream> token_stream;
	std::vector<std::string> filenames;
    };
};
