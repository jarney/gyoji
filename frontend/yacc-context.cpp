#include <jlang-frontend/jsyntax.hpp>

using namespace JLang::frontend::yacc;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::tree;

YaccContext::YaccContext(
                         NamespaceContext & _namespace_context
                         )
  : namespace_context(namespace_context)
  , translation_unit(nullptr)
{}
YaccContext::~YaccContext()
{}

NamespaceContext &
YaccContext::get_namespace_context()
{
  return namespace_context;
}

TranslationUnit_owned_ptr
YaccContext::get_translation_unit()
{
  return std::move(translation_unit);
}

void
YaccContext::set_translation_unit(TranslationUnit_owned_ptr _translation_unit)
{
  translation_unit = std::move(_translation_unit);
}
