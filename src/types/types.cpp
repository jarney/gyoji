#include <jlang-types.hpp>
#include <stdio.h>

using namespace JLang::types;

int JLang::types::doit()
{
  fprintf(stderr, "Doing type resolution");
  return 0;
}
