#include <jlang-frontend.hpp>
#include <jlang-misc/input-source-file.hpp>

#include <jlang-backend/jbackend.hpp>
#include <jlang-backend/jbackend-format-identity.hpp>
#include <jlang-backend/jbackend-format-tree.hpp>

using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::backend;

int main(int argc, char **argv)
{

    if (argc != 2) {
      fprintf(stderr, "Invalid number of arguments %d\n", argc);
      fprintf(stderr, "Usage: parser backend file\n");
      exit(1);
    }
    
    FILE *input = fopen(argv[1], "rb");
    if (input == NULL) {
      fprintf(stderr, "Cannot open file %s\n", argv[1]);
      exit(1);
    }

    CompilerContext context;
    
    JLang::misc::InputSourceFile input_source(input);

    JLang::owned<ParseResult> parse_result = 
        Parser::parse(
                      context,
                      input_source
                      );
    fclose(input);
    if (parse_result->has_errors()) {
      parse_result->get_errors().print();
      return -1;
    }

    const TranslationUnit & translation_unit = parse_result->get_translation_unit();
    
    auto backend = std::make_shared<JBackendFormatIdentity>();
    backend->process(translation_unit.get_syntax_node());

    return 0;
}
