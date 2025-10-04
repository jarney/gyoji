#include <jlang-context.hpp>

using namespace JLang::context;

CompilerContext::CompilerContext(std::string _filename)
{
    token_stream = std::make_unique<TokenStream>();
    errors = std::make_unique<Errors>(*token_stream);
    filenames.push_back(_filename);
}
CompilerContext::~CompilerContext()
{}

bool
CompilerContext::has_errors() const
{ return errors->size(); }

Errors &
CompilerContext::get_errors() const
{ return *errors; }

TokenStream &
CompilerContext::get_token_stream() const
{ return *token_stream; }

const std::string &
CompilerContext::get_filename() const
{ return filenames.back(); }

void
CompilerContext::add_filename(const std::string & _filename)
{
    filenames.push_back(_filename);
}
