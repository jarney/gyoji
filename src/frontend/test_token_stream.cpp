#include <jlang-frontend.hpp>
#include <jlang-frontend/input-source-file.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::errors;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;

/*
 * The purpose of this file is to
 * perform a literal output of the token
 * stream exactly as it was read so that
 * we can verify that all original input
 * is available later when we need it
 * for printing error messages or
 * processing tokens.
 */

static
::JLang::owned<ParseResult>
parse(std::string & filename)
{
  FILE *input = fopen(filename.c_str(), "rb");
  if (input == NULL) {
    fprintf(stderr, "Cannot open file %s\n", filename.c_str());
    return nullptr;
  }
  ::JLang::owned<NamespaceContext> namespace_context = std::make_unique<NamespaceContext>();
  InputSourceFile input_source(input);
  Parser parser(std::move(namespace_context));
  int rc = parser.parse(input_source);
  if (rc != 0) {
    fprintf(stderr, "Syntax error : %s\n", filename.c_str());
    return nullptr;
  }
  ::JLang::owned<ParseResult> parse_result = parser.get_parse_result();
  return std::move(parse_result);
}


int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "First argument must be the path of the source directory\n");
    return -1;
  }

  std::string path(argv[1]);

  ::JLang::owned<ParseResult> result = parse(path);

  for (const ::JLang::owned<Token> & token : result->get_token_stream().get_tokens()) {
    printf("%s", token->get_value().c_str());
  }
  
  return 0;
}
