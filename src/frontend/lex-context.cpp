#include <jlang-frontend.hpp>
#define _JLANG_INTERNAL
#include <lex-context.hpp>
#undef _JLANG_INTERNAL
#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::frontend::yacc;

LexContext::LexContext(
    JLang::frontend::namespaces::NamespaceContext &_namespace_context,
    JLang::context::CompilerContext & _compiler_context,
    JLang::misc::InputSource &_input_source)
    : namespace_context(_namespace_context)
    , input_source(_input_source)
    , compiler_context(_compiler_context)
    , line(1)
    , column(0)
{}

LexContext::~LexContext()
{}
