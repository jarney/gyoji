#include <jlang-frontend/frontend.hpp>

using namespace JLang::frontend;

TokenStream::TokenStream()
{}

TokenStream::~TokenStream()
{}

void
TokenStream::print_from_lex() const
{
  fprintf(stderr, "Printing from lexer\n");
}

void
TokenStream::print_from_yacc() const
{
  fprintf(stderr, "Printing from yacc\n");
}

void
TokenStream::print_from_main() const
{
  fprintf(stderr, "Printing from main\n");
}
