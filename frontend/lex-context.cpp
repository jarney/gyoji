#include <jlang-frontend/frontend.hpp>
#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::frontend::yacc;

LexContext::LexContext(JLang::frontend::namespaces::NamespaceContext &_namespace_context, InputSource &_input_source)
  : namespace_context(_namespace_context)
  , input_source(_input_source)
  , lineno(1)
{}

LexContext::~LexContext()
{}
