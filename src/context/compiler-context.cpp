#include <jlang-context.hpp>

using namespace JLang::context;

CompilerContext::CompilerContext()
{
    token_stream = std::make_unique<TokenStream>();
    errors = std::make_unique<Errors>(*token_stream);
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
