#include <jlang-frontend.hpp>
#include <jlang-misc/input-source-file.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::context;
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
JLang::owned<ParseResult>
parse(std::string & filename, CompilerContext & context)
{
  int input = open(filename.c_str(), O_RDONLY);
  if (input == -1) {
    fprintf(stderr, "Cannot open file %s\n", filename.c_str());
    return nullptr;
  }
  
  JLang::misc::InputSourceFile input_source(input);
  JLang::owned<ParseResult> parse_result = 
    Parser::parse(
                  context,
                  input_source
                  );
  close(input);
  if (parse_result->has_errors()) {
    parse_result->get_errors().print();
    return nullptr;
  }

  return std::move(parse_result);
}


int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "First argument must be the path of the source directory\n");
    return -1;
  }

  std::string path(argv[1]);

  CompilerContext context;
  
  JLang::owned<ParseResult> result = parse(path, context);

  for (const JLang::owned<Token> & token : result->get_token_stream().get_tokens()) {
    printf("%s", token->get_value().c_str());
  }
  
  return 0;
}
