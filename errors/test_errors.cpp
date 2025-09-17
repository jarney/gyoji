#include <jlang-errors/errors.hpp>

using namespace JLang::errors;

int main(int argc, char **argv)
{
  Errors errors;

  std::unique_ptr<Error> error = std::make_unique<Error>("Syntax Error");
  std::vector<std::pair<size_t, std::string>> context;
  context.push_back(std::make_pair<size_t, std::string>(8, "{"));
  context.push_back(std::make_pair<size_t, std::string>(9, "    x = 12;"));
  context.push_back(std::make_pair<size_t, std::string>(10, "   y = 14;"));
  context.push_back(std::make_pair<size_t, std::string>(11, "   p = x + y;"));
  context.push_back(std::make_pair<size_t, std::string>(12, "   a = asdfsdf::23;"));
  context.push_back(std::make_pair<size_t, std::string>(113, "}"));
  error->add_message(context, 12, 4, "Invalid namespace asdfsdf");
  error->print();
  
  errors.add_error(std::move(error));
}
