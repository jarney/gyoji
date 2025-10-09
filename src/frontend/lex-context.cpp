#include <gyoji-frontend.hpp>
#define _GYOJI_INTERNAL
#include <gyoji-frontend/lex-context.hpp>
#undef _GYOJI_INTERNAL
#include <gyoji.l.hpp>
#include <gyoji.y.hpp>

using namespace Gyoji::frontend::yacc;

LexContext::LexContext(
    Gyoji::frontend::namespaces::NamespaceContext &_namespace_context,
    Gyoji::context::CompilerContext & _compiler_context,
    Gyoji::misc::InputSource &_input_source)
    : namespace_context(_namespace_context)
    , input_source(_input_source)
    , compiler_context(_compiler_context)
    , line(1)
    , column(0)
{}

LexContext::~LexContext()
{}
