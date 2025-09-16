#include <jlang-frontend/frontend.hpp>
#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::frontend::yacc;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::tree;

YaccContext::YaccContext(
                         NamespaceContext & _namespace_context
                         )
  : namespace_context(_namespace_context)
  , translation_unit(nullptr)
{}
YaccContext::~YaccContext()
{}

const NamespaceContext &
YaccContext::get_namespace_context() const
{
  return namespace_context;
}

const TranslationUnit &
YaccContext::get_translation_unit() const
{
  return *translation_unit;
}

void
YaccContext::set_translation_unit(TranslationUnit_owned_ptr _translation_unit)
{
  translation_unit = std::move(_translation_unit);
}
