#pragma once

#include <jlang-context/errors.hpp>
#include <jlang-context/token-stream.hpp>
#include <jlang-context/source-reference.hpp>

namespace JLang::context {
    class CompilerContext {
    public:
	CompilerContext(std::string _filename);
	~CompilerContext();
	
	bool has_errors() const;
	Errors & get_errors() const;
	TokenStream & get_token_stream() const;
	const std::string & get_filename() const;
	void add_filename(const std::string & _filename);
    private:
	JLang::owned<Errors> errors;
	JLang::owned<TokenStream> token_stream;
	std::vector<std::string> filenames;
    };
};
