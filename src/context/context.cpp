#include <jlang-context.hpp>

using namespace JLang::context;

CompilerContext::CompilerContext()
  : errors(std::make_unique<Errors>())
  , token_stream(std::make_unique<TokenStream>())
{}
CompilerContext::~CompilerContext()
{}


Errors &
CompilerContext::get_errors() const
{ return *errors; }

TokenStream &
CompilerContext::get_token_stream() const
{ return *token_stream; }
