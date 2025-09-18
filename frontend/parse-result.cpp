#include <jlang-frontend.hpp>
#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::errors;
using namespace JLang::frontend;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::tree;

ParseResult::ParseResult(
                         NamespaceContext_owned_ptr  _namespace_context
                         )
  : namespace_context(std::move(_namespace_context))
  , errors(std::make_unique<Errors>())
  , token_stream(std::make_unique<TokenStream>())
  , translation_unit(nullptr)
{}
ParseResult::~ParseResult()
{}

const NamespaceContext &
ParseResult::get_namespace_context() const
{
  return *namespace_context;
}
const Errors &
ParseResult::get_errors() const
{
  return *errors;
}

const TranslationUnit & 
ParseResult::get_translation_unit() const
{
  return *translation_unit;
}
bool
ParseResult::has_translation_unit() const
{ return translation_unit.get() != nullptr; }

bool
ParseResult::has_errors() const
{
  return errors->size() != 0;
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
