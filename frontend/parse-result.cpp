#include <jlang-frontend.hpp>
#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::frontend;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::tree;

ParseResult::ParseResult(
                         NamespaceContext & _namespace_context,
                         JLang::errors::Errors & _errors
                         )
  : namespace_context(_namespace_context)
  , errors(_errors)
  , token_stream(std::make_unique<TokenStream>())
  , translation_unit(nullptr)
{}
ParseResult::~ParseResult()
{}

const NamespaceContext &
ParseResult::get_namespace_context() const
{
  return namespace_context;
}

const TranslationUnit & 
ParseResult::get_translation_unit() const
{
  return *translation_unit;
}

const TokenStream &
ParseResult::get_token_stream() const
{
  return *token_stream;
}

void
ParseResult::set_translation_unit(TranslationUnit_owned_ptr _translation_unit)
{
  translation_unit = std::move(_translation_unit);
}
