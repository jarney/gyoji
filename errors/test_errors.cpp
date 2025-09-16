#include <jlang-errors/errors.hpp>

using namespace JLang::errors;

int main(int argc, char **argv)
{
  Errors errors;

  errors.add_error("Test error");
}
