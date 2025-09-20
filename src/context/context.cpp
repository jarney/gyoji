#include <jlang-context.hpp>

using namespace JLang::context;

Context::Context()
  : errors(std::make_unique<Errors>())
  , token_stream(std::make_unique<TokenStream>())
{}
Context::~Context()
{}


Errors &
Context::get_errors() const
{ return *errors; }

TokenStream &
Context::get_token_stream() const
{ return *token_stream; }
