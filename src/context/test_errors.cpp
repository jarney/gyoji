#include <jlang-context/errors.hpp>
#include <jlang-context/token-stream.hpp>

using namespace JLang::context;

int main(int argc, char **argv)
{

  TokenStream token_stream;
  size_t lineno = 1;
  token_stream.add_token("none", "{", lineno++, 0);
  token_stream.add_token("none", "    x = 12;", lineno++, 0);
  token_stream.add_token("none", "    a = asdfasdf::23;", lineno++, 0);
  token_stream.add_token("none", "123456789", lineno++, 0);
  token_stream.add_token("none", "    y = 14;", lineno++, 0);
  token_stream.add_token("none", "    p = x + y;", lineno++, 0);
  
  Errors errors(token_stream);

  SourceReference src_ref(std::string("asdf.h"), (size_t)3, (size_t)9);
  std::unique_ptr<Error> error = std::make_unique<Error>("Syntax Error");
  error->add_message(src_ref, "Invalid namespace asdfsdf");
  errors.add_error(std::move(error));
  
  errors.print();
}
