#include <jlang-frontend.hpp>
#include <jlang-frontend/input-source-file.hpp>
#include <jlang-backend/jbackend.hpp>
#include <jlang-backend/jbackend-format-tree.hpp>

using namespace JLang::errors;
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

    ::JLang::owned<NamespaceContext> namespace_context = std::make_unique<NamespaceContext>();
    
    InputSourceFile input_source(input);

    ::JLang::owned<ParseResult> parse_result = 
        Parser::parse(
                      std::move(namespace_context),
                      input_source
                      );
    fclose(input);
    if (parse_result->has_errors()) {
      parse_result->get_errors().print();
      return -1;
    }

    const TranslationUnit & translation_unit = parse_result->get_translation_unit();
    
    auto backend = std::make_shared<JBackendFormatTree>();
    backend->process(translation_unit.get_syntax_node());

    return 0;
}
