#include <jlang-frontend.hpp>
#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::tree;

ParseResult::ParseResult(
                         JLang::context::CompilerContext & _compiler_context,
                         JLang::owned<NamespaceContext>  _namespace_context
                         )
  : namespace_context(std::move(_namespace_context))
  , compiler_context(_compiler_context)
  , translation_unit(nullptr)
{}
ParseResult::~ParseResult()
{}

const NamespaceContext &
ParseResult::get_namespace_context() const
{
  return *namespace_context;
}
Errors &
ParseResult::get_errors() const
{
  return compiler_context.get_errors();
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
  return compiler_context.get_errors().size() != 0;
}

const TokenStream &
ParseResult::get_token_stream() const
{
  return compiler_context.get_token_stream();
}
const JLang::context::CompilerContext &
ParseResult::get_compiler_context() const
{ return compiler_context; }

void
ParseResult::set_translation_unit(JLang::owned<TranslationUnit> _translation_unit)
{
  translation_unit = std::move(_translation_unit);
}
