#include <jlang-errors/errors.hpp>

using namespace JLang::errors;

Errors::Errors()
{}
Errors::~Errors()
{}

void
Errors::add_error(std::string errormsg)
{
  errors.push_back(errormsg);
}
