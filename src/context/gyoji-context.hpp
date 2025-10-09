#pragma once

#include <gyoji-context/errors.hpp>
#include <gyoji-context/token-stream.hpp>
#include <gyoji-context/source-reference.hpp>

namespace Gyoji::context {
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
	Gyoji::owned<Errors> errors;
	Gyoji::owned<TokenStream> token_stream;
	std::vector<std::string> filenames;
    };
};
