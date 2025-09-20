#include <jlang-frontend/input-source-file.hpp>
#include <jlang-frontend.hpp>
#include <jlang-types.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::frontend;
using namespace JLang::frontend::namespaces;
using namespace JLang::types;


static
::JLang::owned<ParseResult>
parse(std::string & path, std::string base_filename);

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "First argument must be the path of the source directory\n");
    return -1;
  }
  printf("Testing types\n");

  std::string path(argv[1]);

  auto parse_result = std::move(parse(path, "tests/type-resolution.j"));

  parse_result->get_namespace_context().namespace_dump();
  
  JLang::owned<Types> types = std::move(resolve_types(*parse_result));

  if (parse_result->has_errors()) {
    parse_result->get_errors().print();
    return -1;
  }
  types->dump();
  
  printf("    PASSED\n");
}


static
::JLang::owned<ParseResult>
parse(std::string & path, std::string base_filename)
{
  std::string filename = path + std::string("/") + base_filename;
  
  FILE *input = fopen(filename.c_str(), "rb");
  if (input == NULL) {
    fprintf(stderr, "Cannot open file %s\n", base_filename.c_str());
    return nullptr;
  }

  ::JLang::owned<NamespaceContext> namespace_context = std::make_unique<NamespaceContext>();
  
  InputSourceFile input_source(input);
  ::JLang::owned<ParseResult> parse_result =
      Parser::parse(std::move(namespace_context),
                   input_source
                   );

  return std::move(parse_result);
}
