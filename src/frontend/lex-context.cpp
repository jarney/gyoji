#include <jlang-frontend.hpp>
#define _JLANG_INTERNAL
#include <lex-context.hpp>
#undef _JLANG_INTERNAL
#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::frontend::yacc;

LexContext::LexContext(JLang::frontend::namespaces::NamespaceContext &_namespace_context,
                       JLang::context::TokenStream & _token_stream,
                       JLang::misc::InputSource &_input_source)
  : namespace_context(_namespace_context)
  , token_stream(_token_stream)
  , input_source(_input_source)
  , line(1)
  , column(1)
{}

LexContext::~LexContext()
{}
